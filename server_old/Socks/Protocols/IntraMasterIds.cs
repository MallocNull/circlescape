using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SockScape {
    public enum kIntraSlaveId {
        InitiationAttempt = 0,
        KeyExchange,
        StatusUpdate
    }

    public enum kIntraMasterId {
        KeyExchange = 1,
        PositiveAck,
        NegativeAck,
        EncryptionError
    }
}
