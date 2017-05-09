using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Square {
    public static class CryptoExtensions {
        public static byte[] SHA1(this string str) {
            return Encoding.UTF8.GetBytes(str).SHA1();
        }

        public static byte[] SHA1(this byte[] bytes) {
            using(var hasher = new SHA1CryptoServiceProvider()) {
                return hasher.ComputeHash(bytes);
            }
        }

        public static byte[] MD5(this string str) {
            return Encoding.UTF8.GetBytes(str).MD5();
        }

        public static byte[] MD5(this byte[] bytes) {
            using(var hasher = new MD5CryptoServiceProvider()) {
                return hasher.ComputeHash(bytes);
            }
        }

        /*private static string ParseRawHash(byte[] hash, kHashReturnType type) {
            switch(type) {
                case kHashReturnType.BASE64:
                    return hash.Base64Encode();
                case kHashReturnType.HEX:
                    return BitConverter.ToString(hash).Replace("-", "");
                case kHashReturnType.RAW:
                default:
                    return hash;
            }
        }*/
    }
}
