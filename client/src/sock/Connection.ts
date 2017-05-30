class Connection {
    private static sock: WebSocket = null;
    private static _isOpen: boolean = false;
    private static onOpenFunc: () => void = null;
    private static onCloseFunc: () => void = null;
    public static get isOpen(): boolean {
        return Connection._isOpen;
    }

    public static open(onOpen: () => void = null): void {
        if(Connection._isOpen)
            return;

        // FLAG replace hard coded url with one loaded from a config file
        Connection.sock = new WebSocket("ws://localhost:6770");
        Connection.sock.binaryType = "arraybuffer";

        Connection.onOpenFunc = onOpen;
        Connection.sock.onopen = Connection.onOpen;
        Connection.sock.onmessage = Connection.onMessage;
        Connection.sock.onclose = Connection.onClose;
    }

    public static send(msg: Packet) {
        Connection.sock.send(msg.getBytes());
    }

    private static onOpen(event: any): void {
        Connection._isOpen = true;
        
        if(Connection.onOpenFunc)
            Connection.onOpenFunc();
    }

    private static onMessage(event: any): void {
        var raw = new Uint8Array(event.data);
        var msg: Packet;
        try { 
            msg = !Cipher.ready ? Packet.fromBytes(raw)
                                : Packet.fromBytes(Cipher.parse(raw));
        } catch(e) {
            close();
            return;
        }

        console.log(msg);
        switch(msg.id) {
            case kPacketId.KeyExchange:
                var response = Key.generateResponsePacket(msg);
                if(Key.succeeded) {
                    Cipher.init(Key.privateKey);
                    Connection.send(response);
                } else
                    CriticalStop.redirect("Could not establish an encrypted connection with the server.");
                break;
            case kPacketId.LoginAttempt:

                break;
            case kPacketId.RegistrationAttempt:

                break;
        }
    }

    private static onClose(event: any): void {
        Connection._isOpen = false;
        Cipher.close();

        if(Connection.onCloseFunc)
            Connection.onCloseFunc();
    }

    public static close(onClose: () => void = null): void {
        if(!Connection._isOpen)
            return;

        Connection.onCloseFunc = onClose;
        Connection.sock.close();
    }
}