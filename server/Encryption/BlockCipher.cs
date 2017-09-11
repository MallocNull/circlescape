using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using Glove;

namespace SockScape.Encryption {
    class BlockCipher {
        public const int KeySize = 192;
        public const int KeySizeBytes = KeySize / 8;

        private readonly byte[] Key = new byte[KeySizeBytes];
        private readonly byte[] IV;

        public BlockCipher(BigInteger key) {
            Array.Copy(key.ToByteArray(), Key, Key.Length);

            IV = Configuration.General["Master IV"].Str.HexStringToBytes() 
              ?? new byte[] { 0x0b, 0xfc, 0xd7, 0x2d, 0x23, 0xb7, 0x83, 0xb2 };
        }

        public byte[] Encrypt(byte[] data) {
            try {
                var ms = new MemoryStream();
                var cs = new CryptoStream(ms,
                    new TripleDESCryptoServiceProvider().CreateEncryptor(Key, IV), 
                    CryptoStreamMode.Write);

                cs.Write(data, 0, data.Length);
                cs.FlushFinalBlock();

                byte[] ret = ms.ToArray();

                cs.Close();
                ms.Close();

                return ret;
            } catch(Exception e) {
                Console.WriteLine($"TDES ENCRYPT ERROR: {e.Message}");
                return null;
            }
        }

        public byte[] Decrypt(byte[] data) {
            try {
                var ms = new MemoryStream(data);
                var cs = new CryptoStream(ms,
                    new TripleDESCryptoServiceProvider().CreateDecryptor(Key, IV),
                    CryptoStreamMode.Read);

                byte[] ret = new byte[data.Length];
                cs.Read(ret, 0, data.Length);

                cs.Close();
                ms.Close();

                return ret;
            } catch(Exception e) {
                Console.WriteLine($"TDES DECRYPT ERROR: {e.Message}");
                return null;
            }
        }
    }
}
