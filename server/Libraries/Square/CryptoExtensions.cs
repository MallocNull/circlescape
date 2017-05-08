using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Square {
    public static class CryptoExtensions {
        public enum kHashReturnType {
            RAW, HEX, BASE64
        }

        public static string SHA1(this string str, kHashReturnType type = kHashReturnType.RAW) {
            using(var hasher = new SHA1CryptoServiceProvider()) {
                return ParseRawHash(
                    hasher.ComputeHash(str.GetBytes(false)),
                    type
                );
            }
        }

        public static string MD5(this string str, kHashReturnType type = kHashReturnType.RAW) {
            using(var hasher = new MD5CryptoServiceProvider()) {
                return ParseRawHash(
                    hasher.ComputeHash(str.GetBytes(false)),
                    type
                );
            }
        }

        private static string ParseRawHash(byte[] hash, kHashReturnType type) {
            switch(type) {
                case kHashReturnType.BASE64:
                    return hash.Base64Encode(false);
                case kHashReturnType.HEX:
                    return BitConverter.ToString(hash).Replace("-", "");
                case kHashReturnType.RAW:
                default:
                    return hash;
            }
        }
    }
}
