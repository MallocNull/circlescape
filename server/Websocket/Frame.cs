using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.Websocket {
    class Frame {
        public enum kOpcode {
            Continuation    = 0x0,
            TextFrame       = 0x1,
            BinaryFrame     = 0x2,
            Close           = 0x8,
            Ping            = 0x9,
            Pong            = 0xA
        };

        public kOpcode Opcode { get; private set; }
        public bool IsFinal { get; private set; }
        public bool IsMasked { get; private set; }
        public byte[] Mask { get; private set; }
        public byte Reserved { get; private set; }

    }
}
