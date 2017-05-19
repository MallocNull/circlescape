using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using Square;

namespace CircleScape.Encryption {
    class KeyExchange {
        private BigInteger Secret;
        public BigInteger Generator { get; private set; } = 2;
        public BigInteger Modulus { get; private set; }
        public BigInteger PrivateKey { get; private set; } = BigInteger.MinusOne;

        public KeyExchange() {
            Secret = RNG.NextPrime(512 / 8);
            Modulus = RNG.NextPrime(512 / 8);
        }

        public Packet GenerateInitialPacket() {

            return 
        }
    }
}
