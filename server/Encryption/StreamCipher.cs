using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Glove;

namespace SockScape.Encryption {
    class StreamCipher {
        private readonly byte[] Key = new byte[Encryption.Key.KeySizeBytes];
        private readonly byte[] State = new byte[256];

        public StreamCipher(BigInteger key) {
            int i = 0, j = 0;
            State = State.Select(x => (byte)i++).ToArray();
            Key = Key.Select(x => (byte)0).ToArray();
            Array.Copy(key.ToByteArray(), Key, Key.Length);

            for(i = 0; i < State.Length; ++i) {
                j = (j + State[i] + Key[i % Key.Length]) % 256;
                Utils.Swap(ref State[i], ref State[j]);
            }

            GenerateStream(1024);
        }

        private byte[] GenerateStream(long length) {
            var stream = new byte[length];
            int i = 0, j = 0;

            for(long x = 0; x < length; ++x) {
                i = (i + 1) % 256;
                j = (j + State[i]) % 256;
                Utils.Swap(ref State[i], ref State[j]);
                stream[x] = State[(State[i] + State[j]) % 256];
            }

            return stream;
        }

        public byte[] Parse(byte[] data) {
            var stream = GenerateStream(data.LongLength);
            for(long i = 0; i < data.LongLength; ++i)
                data[i] ^= stream[i];

            return data;
        }
    }
}
