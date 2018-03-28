type ConnEvent = (conn: Connection) => void;
type ConnErrorEvent = (event: any, conn: Connection) => void;
type PacketEvent = (data: Packet, conn: Connection) => void;
type PacketHandle = { id: number, event: PacketEvent };

class Connection {
    private sock: WebSocket = null;
    private _isOpen: boolean = false;

    private address: string;
    private useCipher: boolean;
    private handles: PacketEvent[] = [];

    private onOpenFunc: ConnEvent = null;
    private onCloseFunc: ConnEvent = null;
    private onErrorFunc: ConnErrorEvent = null;
    
    public get isOpen(): boolean {
        return this._isOpen;
    }

    public constructor(address: string, handles: PacketHandle[],
        useCipher: boolean = false, onOpen: ConnEvent = null,
        onClose: ConnEvent = null, onError: ConnErrorEvent = null)
    {
        this.address = address;
        this.useCipher = useCipher;

        this.onOpenFunc = onOpen;
        this.onCloseFunc = onClose;
        this.onErrorFunc = onError;

        handles.forEach(element => {
            this.handles[element.id] = element.event;
        });
    }

    public open(): void {
        if(this._isOpen)
            return;

        this.sock = new WebSocket(this.address);
        this.sock.binaryType = "arraybuffer";

        this.sock.onopen = this.onOpen;
        this.sock.onmessage = this.onMessage;
        this.sock.onerror = this.onError;
        this.sock.onclose = this.onClose;
    }

    public send(msg: Packet) {
        this.sock.send(msg.getBytes());
    }

    private onOpen(event: any): void {
        this._isOpen = true;
        
        if(this.onOpenFunc)
            this.onOpenFunc(this);
    }

    private onMessage(event: any): void {
        var raw = new Uint8Array(event.data);
        var msg: Packet;
        try {                                                                                                                                                                                                           
            msg = (!this.useCipher || !Cipher.ready)
                ? Packet.fromBytes(raw)
                : Packet.fromBytes(Cipher.parse(raw));
        } catch(e) {
            close();
            return;
        }

        console.log(msg);
        if(msg.id < this.handles.length && this.handles[msg.id] !== undefined)
            this.handles[msg.id](msg, this);

        /*
        switch(msg.id) {
            case kMasterId.KeyExchange:
                break;
            case kMasterId.LoginAttempt:

                break;
            case kMasterId.RegistrationAttempt:

                break;
        }
        */
    }

    private onError(event: any): void {
        if(this.onErrorFunc)
            this.onErrorFunc(event, this);
    }

    private onClose(event: any): void {
        this._isOpen = false;
        Cipher.close();

        if(this.onCloseFunc)
            this.onCloseFunc(this);
    }

    public close(): void {
        if(!this._isOpen)
            return;

        this.sock.close();
    }
}