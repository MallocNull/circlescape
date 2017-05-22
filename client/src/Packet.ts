const enum kPacketId {
    KeyExchange = 0,
    LoginAttempt,
    RegistrationAttempt
}

class Packet {
    private _Id: kPacketId;
    public get Id(): kPacketId {
        return this._Id;
    }

    private _IsLegal: boolean = true;
    public get IsLegal(): boolean {
        return this._IsLegal;
    }

    private Regions: Uint8Array[] = [];
    
    public get RegionCount(): number {
        return this.Regions.length;
    }
}