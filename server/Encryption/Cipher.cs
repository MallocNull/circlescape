using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.Encryption {
    class Cipher {

        static void ksa(byte[] state, byte[] key) {
            int i, j = 0, t;
            for(i = 0; i < 256; ++i)
                state[i] = (byte)i;

            for(i = 0; i < 256; ++i) {
                j = (j + state[i] + key[i % key.Length]) % 256;
                t = state[i];
                state[i] = state[j];
                state[j] = (byte)t;
            }
        }

        static void prga(byte[] state, byte[] cipher) {
            int i = 0, j = 0, x, t;

            for(x = 0; x < cipher.Length; ++x) {
                i = (i + 1) % 256;
                j = (j + state[i]) % 256;
                t = state[i];
                state[i] = state[j];
                state[j] = (byte)t;
                cipher[x] = state[(state[i] + state[j]) % 256];
            }
        }
    }
}
