using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Kneesocks {
    public static class StringExtensions {
        public enum kHashReturnType {
            RAW, HEX, BASE64
        }

        public static string Base64Encode(this string str, bool isUtf8 = true) {
            var raw =
            isUtf8 ? Encoding.UTF8.GetBytes(str)
                    : Encoding.ASCII.GetBytes(str);
            return Convert.ToBase64String(raw);
        }

        public static string Base64Decode(this string str, bool isUtf8 = true) {
            var raw = Convert.FromBase64String(str);
            return isUtf8 ? Encoding.UTF8.GetString(raw)
                            : Encoding.ASCII.GetString(raw);
        }

        public static string SHA1(this string str, kHashReturnType type = kHashReturnType.RAW) {
            using(var hasher = System.Security.Cryptography.SHA1.Create()) {
                return ParseRawHash(
                    hasher.ComputeHash(Encoding.ASCII.GetBytes(str)),
                    type
                );
            }
        }

        public static string MD5(this string str, kHashReturnType type = kHashReturnType.RAW) {
            using(var hasher = System.Security.Cryptography.MD5.Create()) {
                return ParseRawHash(
                    hasher.ComputeHash(Encoding.ASCII.GetBytes(str)),
                    type
                );
            }
        }

        private static string ParseRawHash(byte[] hash, kHashReturnType type) {
            var raw = Encoding.ASCII.GetString(hash);

            switch(type) {
                case kHashReturnType.BASE64:
                    return Base64Encode(raw, false);
                case kHashReturnType.HEX:
                    return BitConverter.ToString(hash).Replace("-", "");
                case kHashReturnType.RAW:
                default:
                    return raw;
            }
        }
    }
}
