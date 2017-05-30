class Key {
    private static secret: bigInt;
    private static _privateKey: bigInt = new bigInt(0);
    private static get privateKey(): bigInt {
        return Key._privateKey;
    }

    public static get succeeded(): boolean {
        return !Key._privateKey.eq(new bigInt(0));
    }

    public static init(): void { 
        Key.secret = Random.generatePrime(512);
    }

    public static generateResponsePacket(request: Packet): Packet {
        var generator = new bigInt(request[0].toString(), 16);
        var modulus = new bigInt(request[1].toString(), 16);
        var serverKey = new bigInt(request[2].toString(), 16);

        Key._privateKey = serverKey.modPow(serverKey, modulus);
        return Packet.create(kPacketId.KeyExchange, [generator.modPow(Key.secret, modulus).toString(16)]);
    }
}

class Cipher {
    private static key: Uint8Array;
    private static state: Uint8Array;

    public static init(key: bigInt) {
        Cipher.key = key.toByteArray(512 / 8);
        Cipher.state = new Uint8Array(256);
        Cipher.state.map((value: number, index: number): number => {
            return index;
        });
        
        var i, j = 0, t;
        for(i = 0; i < 256; ++i) {
            j = (j + Cipher.state[i] + Cipher.key[i % Cipher.key.length]) % 256;

            t = Cipher.state[i];
            Cipher.state[i] = Cipher.state[j];
            Cipher.state[j] = t;
        }

        Cipher.generateStream(1024);
    }

    private static generateStream(length: number): Uint8Array {
        var stream = new Uint8Array(length);
        var i = 0, j = 0, x, t;

        for(x = 0; x < length; ++x) {
            i = (i + 1) % 256;
            j = (j + Cipher.state[i]) % 256;

            t = Cipher.state[i];
            Cipher.state[i] = Cipher.state[j];
            Cipher.state[j] = t;

            stream[x] = Cipher.state[(Cipher.state[i] + Cipher.state[j]) % 256];
        }

        return stream;
    }

    public static parse(data: Uint8Array): Uint8Array {
        var stream = Cipher.generateStream(data.length);
        for(var i = 0; i < data.length; ++i)
            data[i] = data[i] ^ stream[i];

        return data;
    }
}