const enum kMasterId {
    KeyExchange = 1,
    LoginAttempt,
    RegistrationAttempt,
    ServerListing
}

const enum kSlaveId {
    
}

class Packet {
    private static magicNumber: Uint8Array = new Uint8Array([0xF0, 0x9F, 0xA6, 0x91]);

    private _id: number;
    public get id(): number {
        return this._id;
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
    public addRegion(region: object): Packet {
        if(typeof region == "string")
            this._regions.push((<string>region).toByteArray());
        else if(region instanceof Uint8Array)
            this._regions.push(region);

        this[this.regionCount-1] = this._regions[this.regionCount-1];
        return this;
    }

    private constructor() {}

    public static create(id: number, regions: any[]): Packet {
        var packet = new Packet;
        packet._id = id;
        regions.forEach(region => {
            packet.addRegion(region);
        });

        return packet;
    }

    public static fromBytes(bytes: Uint8Array): Packet {
        var packet = new Packet;

        if(!bytes.subarray(0, 4).every((v, i) => v === Packet.magicNumber[i]))
            return null;

        packet._id = bytes[4];
        var regionCount = bytes[5];
        var regionLengths = [];
        var ptr = 6;
        for(var i = 0; i < regionCount; ++i) {
            if(bytes[ptr] < 0xFE)
                regionLengths.push(bytes[ptr]);
            else if(bytes[ptr] == 0xFE) {
                regionLengths.push(bytes.unpackUint16(ptr + 1));
                ptr += 2;
            } else {
                regionLengths.push(bytes.unpackUint32(ptr + 1));
                ptr += 4;
            }

            ++ptr;
        }

        for(var i = 0; i < regionCount; ++i) {
            packet.addRegion(bytes.subarray(ptr, ptr + regionLengths[i]));
            ptr += regionLengths[i];
        }

        return packet;
    }

    public getBytes(): Uint8Array {
        var headerSize = 6, bodySize = 0;
        this._regions.forEach(region => {
            bodySize += region.byteLength;

            ++headerSize;
            if(region.byteLength >= 0xFE && region.byteLength <= 0xFFFF)
                headerSize += 2;
            else if(region.byteLength > 0xFFFF)
                headerSize += 4;
        });

        var buffer = new Uint8Array(headerSize + bodySize);
        var headerPtr = 6, bodyPtr = headerSize;
        buffer.set(Packet.magicNumber, 0);
        buffer[4] = this._id % 256;
        buffer[5] = this._regions.length;
        this._regions.forEach(region => {
            var regionLength = region.byteLength;
            if(regionLength < 0xFE)
                buffer[headerPtr] = regionLength;
            else if(regionLength >= 0xFE && regionLength <= 0xFFFF) {
                buffer[headerPtr] = 0xFE;
                buffer.set(regionLength.packUint16(), headerPtr + 1);
                headerPtr += 2;
            } else {
                buffer[headerPtr] = 0xFF;
                buffer.set(regionLength.packUint32(), headerPtr + 1);
                headerPtr += 4;
            }
            ++headerPtr;

            buffer.set(region, bodyPtr);
            bodyPtr += regionLength;
        });
        
        return buffer;
    }
}