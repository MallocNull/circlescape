using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Square {
    public static class NumericExtensions {
        public static byte[] Pack(this Single value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }

        public static byte[] Pack(this Double value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }

        public static byte[] Pack(this Int16 value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }

        public static byte[] Pack(this UInt16 value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }

        public static byte[] Pack(this Int32 value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }

        public static byte[] Pack(this UInt32 value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }
        
        public static byte[] Pack(this Int64 value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }

        public static byte[] Pack(this UInt64 value) {
            return BitConverter.GetBytes(value).HostToNetworkOrder();
        }
    }
}
