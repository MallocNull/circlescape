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
        var headerSize = 1, bodySize = 0;
        this._regions.forEach(region => {
            bodySize += region.byteLength;

            ++headerSize;
            if(region.byteLength >= 254 && region.byteLength <= 0xFFFF)
                headerSize += 2;
            else
                headerSize += 4;
        });

        var buffer = new Uint8Array(headerSize + bodySize);
        var headerPtr = 1, bodyPtr = 0;
        buffer[0] = this._id % 256;
        this._regions.forEach(region => {
            
        });
        
        return buffer;
    }
}