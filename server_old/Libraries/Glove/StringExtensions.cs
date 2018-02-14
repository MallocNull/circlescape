using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Glove {
    public static class CharExtensions {
        public static bool IsHex(this char c) {
            return (c >= '0' && c <= '9')
                   || (c >= 'A' && c <= 'F')
                   || (c >= 'a' && c <= 'f');
        }

        public static byte HexValue(this char c) {
            return (byte)(!c.IsHex() ? 0 : c - (c < 0x3A ? 0x30 : (c < 0x61 ? 0x37 : 0x57)));
        }
    }

    public static class StringExtensions {
        public static byte[] GetBytes(this string str, bool isUtf8 = true)
            => isUtf8 ? Encoding.UTF8.GetBytes(str)
                      : Encoding.ASCII.GetBytes(str);

        public static int ByteLength(this string str, bool isUtf8 = true)
            => isUtf8 ? Encoding.UTF8.GetByteCount(str)
                      : Encoding.ASCII.GetByteCount(str);

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

        public static byte[] Base64DecodeRaw(this string str)
            => Convert.FromBase64String(str);

        public static bool StartsWith(this string str, params string[] strings) {
            foreach(var checkString in strings) {
                if(str.StartsWith(checkString))
                    return true;
            }

            return false;
        }
    }
}
