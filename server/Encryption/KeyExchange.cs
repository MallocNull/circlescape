using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Glove;
using System.Globalization;

namespace SockScape.Encryption {
    class Key {
        private static readonly BigInteger Secret = RNG.NextPrime(512 / 8);
        public BigInteger Generator { get; } = 2;
        public BigInteger Modulus { get; }
        public BigInteger PrivateKey { get; private set; } = BigInteger.Zero;
        public bool Succeeded 
            => !PrivateKey.IsZero;

        public Key() {
            Modulus = RNG.NextPrime(512 / 8);
        }

        public Packet GenerateRequestPacket() {
            return new Packet(
                1, 
                Generator.ToHexString(), 
                Modulus.ToHexString(), 
                BigInteger.ModPow(Generator, Secret, Modulus).ToHexString()
            );
        }

        public Packet ParseRequestPacket(Packet packet) {
            if(packet.Id != 1 || packet.RegionCount != 3)
                return null;

            var check = BigInteger.TryParse(packet[0], NumberStyles.HexNumber, 
                NumberFormatInfo.InvariantInfo, out BigInteger generator);
            check &= BigInteger.TryParse(packet[1], NumberStyles.HexNumber,
                NumberFormatInfo.InvariantInfo, out BigInteger modulus);
            check &= BigInteger.TryParse(packet[2], NumberStyles.HexNumber,
                NumberFormatInfo.InvariantInfo, out BigInteger serverKey);

            if(!check)
                return null;

            var clientKey = BigInteger.ModPow(generator, Secret, modulus);
            PrivateKey = BigInteger.ModPow(serverKey, Secret, modulus);
            return new Packet(1, clientKey.ToHexString());
        }

        public BigInteger ParseResponsePacket(Packet packet) {
            if(packet.Id != 1 || packet.RegionCount != 1)
                return -1;

            if(!BigInteger.TryParse(packet[0], NumberStyles.HexNumber, NumberFormatInfo.InvariantInfo, out BigInteger clientKey))
                return -1;

            return PrivateKey = BigInteger.ModPow(clientKey, Secret, Modulus);
        }
    }
}
