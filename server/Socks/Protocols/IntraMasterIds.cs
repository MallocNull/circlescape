using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SockScape {
    public enum kIntraMasterId {
        InitiationAttempt = 0,
        KeyExchange,
        StatusUpdate
    }

    public enum kIntraMasterAckId {
        PositiveAck = 1,
        NegativeAck
    }
}
