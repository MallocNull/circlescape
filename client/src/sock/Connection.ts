class Connection {
    private sock: WebSocket = null;
    private _isOpen: boolean = false;
    private onOpenFunc: () => void = null;
    private onCloseFunc: () => void = null;
    public get isOpen(): boolean {
        return this._isOpen;
    }

    public open(onOpen: () => void = null): void {
        if(this._isOpen)
            return;

        // FLAG replace hard coded url with one loaded from a config file
        this.sock = new WebSocket("ws://localhost:6770");
        this.sock.binaryType = "arraybuffer";

        this.onOpenFunc = onOpen;
        this.sock.onopen = this.onOpen;
        this.sock.onmessage = this.onMessage;
        this.sock.onclose = this.onClose;
    }

    public send(msg: Packet) {
        this.sock.send(msg.getBytes());
    }

    private onOpen(event: any): void {
        this._isOpen = true;
        
        if(this.onOpenFunc)
            this.onOpenFunc();
    }

    private onMessage(event: any): void {
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
            case kMasterId.KeyExchange:
                var response = Key.generateResponsePacket(msg);
                if(Key.succeeded) {
                    Cipher.init(Key.privateKey);
                    this.send(response);
                } else
                    CriticalStop.redirect("Could not establish an encrypted connection with the server.");
                break;
            case kMasterId.LoginAttempt:
                
                break;
            case kMasterId.RegistrationAttempt:

                break;
        }
    }

    private onClose(event: any): void {
        this._isOpen = false;
        Cipher.close();

        if(this.onCloseFunc)
            this.onCloseFunc();
    }

    public close(onClose: () => void = null): void {
        if(!this._isOpen)
            return;

        this.onCloseFunc = onClose;
        this.sock.close();
    }
}