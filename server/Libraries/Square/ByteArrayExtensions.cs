using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Square {
    public static class ByteArrayExtensions {
        public static string Base64Encode(this byte[] bytes) {
            return Convert.ToBase64String(bytes);
        }
    }
}
