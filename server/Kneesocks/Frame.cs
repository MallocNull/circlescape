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
        public byte[] Mask { get; set; } = new byte[] { 0, 0, 0, 0 };
        public byte Reserved { get; set; }
        
        public byte[] Content { get; set; }
        public byte[] MaskedContent {
            get {
                byte[] returnValue = new byte[Content.Length];
                for(var i = 0; i < Content.Length; ++i)
                    returnValue[i] = (byte)(Content[i] ^ Mask[i % 4]);
                return returnValue;
            }
        }

        public static Frame FromRaw(byte[] raw) {
            if(raw.Length < 2)
                throw new FormatException("Websocket frame cannot be less than two bytes long");

            var rawOpcode = raw[0] & 0x0F;
            if(!Enum.IsDefined(typeof(kOpcode), rawOpcode))
                throw new FormatException("Opcode '"+ rawOpcode.ToString("0x{0:X}") +"' not understood");
            
            var returnFrame = new Frame {
                IsFinal = (raw[0] & 0x80) != 0,
                Opcode = (kOpcode)rawOpcode,
                IsMasked = (raw[1] & 0x80) != 0,
                Reserved = (byte)((raw[0] & 0x70) >> 4)
            };

            UInt64 frameLength = raw[1] & 0x7Ful;
            int lengthOffset = 
                frameLength < 126 
                    ? 1 
                    : (frameLength == 126 ? 3 : 9);
            if()

            return returnFrame;
        }
    }
}
