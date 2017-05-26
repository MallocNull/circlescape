using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Square;

namespace CircleScape.Encryption {
    class Key {
        private BigInteger Secret;
        public BigInteger Generator { get; private set; } = 2;
        public BigInteger Modulus { get; private set; }
        public BigInteger PrivateKey { get; private set; } = BigInteger.Zero;
        public bool Succeeded {
            get => !PrivateKey.IsZero;
        }

        public Key() {
            Secret = RNG.NextPrime(512 / 8);
            Modulus = RNG.NextPrime(512 / 8);
        }

        public Packet GenerateRequestPacket() {
            return new Packet(
                Packet.kId.KeyExchange, 
                Generator.ToHexString(), 
                Modulus.ToHexString(), 
                BigInteger.ModPow(Generator, Secret, Modulus).ToHexString()
            );
        }

        public BigInteger ParseResponsePacket(Packet packet) {
            if(packet.Id == Packet.kId.KeyExchange && packet.RegionCount != 1)
                return -1;

            if(!BigInteger.TryParse(packet[0], out BigInteger ClientKey))
                return -1;
            
            return (PrivateKey = BigInteger.ModPow(ClientKey, Secret, Modulus));
        }
    }
}
