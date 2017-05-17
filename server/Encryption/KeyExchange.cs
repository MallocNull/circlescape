using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;

namespace CircleScape.Encryption {
    class KeyExchange {
        private BigInteger Secret;
        public BigInteger Generator { get; private set; }
        public BigInteger Modulus { get; private set; }
        public BigInteger PrivateKey { get; private set; }

        // https://security.stackexchange.com/questions/45963/diffie-hellman-key-exchange-in-plain-english/45971#45971
    }
}
