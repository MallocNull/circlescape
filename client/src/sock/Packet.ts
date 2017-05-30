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

    public static create(id: kPacketId, regions: any[]): Packet {
        var packet = new Packet;
        packet._id = id;
        regions.forEach(region => {
            packet.addRegion(region);
        });

        return packet;
    }

    public static fromBytes(bytes: Uint8Array): Packet {
        var packet = new Packet;
        packet._id = bytes[0];
        var regionCount = bytes[1];
        var regionLengths = [];
        var ptr = 2;
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
        var headerSize = 2, bodySize = 0;
        this._regions.forEach(region => {
            bodySize += region.byteLength;

            ++headerSize;
            if(region.byteLength >= 0xFE && region.byteLength <= 0xFFFF)
                headerSize += 2;
            else if(region.byteLength > 0xFFFF)
                headerSize += 4;
        });

        var buffer = new Uint8Array(headerSize + bodySize);
        var headerPtr = 2, bodyPtr = headerSize;
        buffer[0] = this._id % 256;
        buffer[1] = this._regions.length;
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