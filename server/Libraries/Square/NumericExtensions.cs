using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using System.Globalization;

namespace Square {
    public static class NumericExtensions {
        public static byte[] Pack(this Single value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this Double value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this Int16 value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this UInt16 value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this Int32 value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this UInt32 value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this Int64 value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static byte[] Pack(this UInt64 value)
            => BitConverter.GetBytes(value).HostToNetworkOrder();

        public static bool IsDivisibleBy(this BigInteger value, BigInteger dividend) {
            if(value.IsZero) return false;
            if(value.IsOne) return true;
            if(value == 2) return value.IsEven;
            return (value % dividend).IsZero;
        }

        public static bool IsProbablePrime(this BigInteger value, int iterations = 5) {
            var absValue = BigInteger.Abs(value);

            if(absValue == 1) return false;
            if(absValue == 2 || absValue == 3 || absValue == 5) return true;
            if(absValue.IsEven || absValue.IsDivisibleBy(3) || absValue.IsDivisibleBy(5)) return false;
            if(absValue < 25) return true;

            for(var i = 0; i < iterations; ++i) {
                var rand = RNG.NextBigInt(2, absValue - 2);
                if(!BigInteger.ModPow(rand, absValue - 1, absValue).IsOne) return false;
            }

            return true;
        }

        public static string ToHexString(this BigInteger value)
            => value.ToString("X").ToLower();

        public static BigInteger HexStringToBigInt(this string value)
            => BigInteger.Parse(value, NumberStyles.HexNumber);
    }
}
