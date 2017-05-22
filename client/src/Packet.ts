const enum kPacketId {
    KeyExchange = 0,
    LoginAttempt,
    RegistrationAttempt
}

class Packet {
    private _id: kPacketId;
    public get id(): kPacketId {
        return this._id;
    }

    private _isLegal: boolean = true;
    public get isLegal(): boolean {
        return this._isLegal;
    }

    private _regions: Uint8Array[] = [];
    public get regions(): Uint8Array[] {
        return this._regions;
    }
    public get regionCount(): number {
        return this._regions.length;
    }
    public getRegion(region: number): Uint8Array {
        return this._regions[region];
    }
    public getRegionString(region: number): string {
        return this.getRegion(region).toString();
    }

    public getBytes(): Uint8Array {
        var messageSize = 1;
        this._regions.forEach(region => {
            messageSize += region.byteLength + 1;
            if(region.byteLength >= 254 && region.byteLength <= 0xFFFF)
                messageSize += 2;
            else
                messageSize += 4;
        });

        var buffer = new Uint8Array(messageSize);
        this._regions.forEach(region => {
            
        });
    }
}