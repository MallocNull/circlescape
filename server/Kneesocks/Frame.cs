using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Kneesocks {
    public class Frame {
        public enum kOpcode {
            Continuation    = 0x0,
            TextFrame       = 0x1,
            BinaryFrame     = 0x2,
            Close           = 0x8,
            Ping            = 0x9,
            Pong            = 0xA
        };

        public kOpcode Opcode { get; set; }
        public bool IsFinal { get; set; }
        public bool IsMasked { get; set; }
        public byte[] Mask { get; set; }
        public byte Reserved { get; set; }


        public byte[] Content { get; private set; }
    }
}
