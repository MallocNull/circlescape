using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Kneesocks {
    public class Frame {
        public enum kClosingReason {
            Normal          = 1000,
            GoingAway       = 1001,
            ProtocolError   = 1002,
            FrameTypeError  = 1003,
            DataError       = 1007,
            PolicyError     = 1008,
            FrameTooBig     = 1009,
            RequestError    = 1011
        }

        public enum kOpcode {
            Continuation    = 0x0,
            TextFrame       = 0x1,
            BinaryFrame     = 0x2,
            Close           = 0x8,
            Ping            = 0x9,
            Pong            = 0xA
        };

        public kOpcode Opcode { get; set; } = kOpcode.BinaryFrame;
        public bool IsFinal { get; set; } = true;
        public bool IsMasked { get; set; } = false;
        public byte[] Mask { get; set; } = new byte[] { 0, 0, 0, 0 };
        public byte Reserved { get; set; } = 0;

        private int _HeaderLength = 0;
        public int HeaderLength {
            get {
                if(_HeaderLength != 0)
                    return _HeaderLength;

                int length = 2
                    + (BodyLength >= 0x7E && BodyLength <= 0xFFFF ? 2 : 0)
                    + (BodyLength > 0xFFFF                        ? 4 : 0)
                    + (IsMasked                                   ? 4 : 0);

                return (_HeaderLength = length);
            }
        }

        private int _BodyLength = 0;
        public int BodyLength {
            get {
                return Content == null ? _BodyLength : Content.Length;
            }
        }

        public byte[] Content { get; set; } = null;
        public byte[] MaskedContent {
            get {
                long counter = 0;
                return Content.Select(x => (byte)(x ^ Mask[counter++ % 4])).ToArray();
            }
        }

        public byte[] GetBytes() {
            var headerSize = 2;
            var bodySize = (UInt64)Content.LongLength;
            var headerLengthFirstByte = (byte)Content.Length;
            if(bodySize >= 0x7E && bodySize <= 0xFFFF) {
                headerSize += 2;
                headerLengthFirstByte = 126;
            } else if(bodySize > 0xFFFF) {
                headerSize += 8;
                headerLengthFirstByte = 127;
            }
            var headerLengthSize = (int)headerSize - 1;
            if(IsMasked)
                headerSize += 4;

            var returnValue = new byte[(UInt64)headerSize + bodySize];
            returnValue[0] = (byte)(((byte)Opcode % 0x10) 
                                  | ((Reserved % 8) << 4) 
                                  |  (IsFinal ? 0x80 : 0x0));

            returnValue[1] = (byte)(headerLengthFirstByte 
                                 | (IsMasked ? 0x80 : 0x0));

            if(headerLengthFirstByte >= 0x7E) {
                var upperBound = headerLengthFirstByte == 0x7E ? 2 : 8;
                for(var i = 0; i < upperBound; ++i)
                    returnValue[2 + i] |= (byte)((bodySize >> (8*(upperBound - i))) & 0xFF);
            }

            if(IsMasked)
                Array.Copy(Mask, 0, returnValue, headerSize - 4, 4);

            Array.Copy(IsMasked ? MaskedContent : Content, 0L, returnValue, headerSize, Content.LongLength);
            return returnValue;
        }

        public static int HeaderSizeFromBytes(byte[] raw) {
            if(raw.Length < 2)
                throw new FormatException("Need first two bytes to analyze");

            var lengthByte = raw[1] & 0x7F;
            return 2 
                + ((raw[1] & 0x80) != 0 ? 4: 0)
                +  (lengthByte == 0x7E ? 2 : 0) 
                +  (lengthByte == 0x7F ? 4 : 0);
        }

        public static Frame HeaderFromBytes(byte[] raw) {
            if(raw.Length < 2)
                throw new FormatException("Websocket frame cannot be less than two bytes long");

            var rawOpcode = raw[0] & 0x0F;
            if(!Enum.IsDefined(typeof(kOpcode), rawOpcode))
                throw new ArgumentException("Opcode '0x" + rawOpcode.ToString("X") + "' not understood");

            var returnFrame = new Frame {
                IsFinal  = (raw[0] & 0x80) != 0,
                Opcode   = (kOpcode)rawOpcode,
                IsMasked = (raw[1] & 0x80) != 0,
                Reserved = (byte)((raw[0] & 0x70) >> 4)
            };

            ulong bodyLength = raw[1] & 0x7Ful;
            int headerOffset =
                bodyLength < 0x7E
                    ? 1
                    : (bodyLength == 0x7E ? 3 : 9);

            if(raw.Length < headerOffset + 1)
                throw new FormatException("Websocket frame is smaller than expected header size");

            bodyLength = bodyLength < 0x7E ? 0 : bodyLength;
            for(var i = headerOffset - 1; i > 0; --i)
                bodyLength |= (ulong)raw[2 + i] << (8 * (headerOffset - 1 - i));

            if(bodyLength > Int32.MaxValue)
                throw new FormatException("Frame is too large to interpret");

            returnFrame._BodyLength = (int)bodyLength;

            if(returnFrame.IsMasked)
                Array.Copy(raw, headerOffset + 1, returnFrame.Mask, 0, 4);

            return returnFrame;
        }

        public static Frame FromBytes(byte[] raw) {
            var returnFrame = HeaderFromBytes(raw);

            uint expectedFrameLength = (uint)returnFrame.BodyLength + (uint)returnFrame.HeaderLength;
            if(expectedFrameLength < (uint)raw.Length)
                throw new FormatException("Raw frame length ("+ (uint)raw.Length + ") is less than described size ("+ expectedFrameLength + ")");

            returnFrame.Content = new byte[returnFrame.BodyLength];
            Array.Copy(raw, returnFrame.HeaderLength, returnFrame.Content, 0L, (long)returnFrame.BodyLength);
            if(returnFrame.IsMasked)
                returnFrame.Content = returnFrame.MaskedContent;

            return returnFrame;
        }

        public static Frame Closing(kClosingReason status = kClosingReason.Normal, string reason = "") {
            var data = new byte[2 + reason.ByteLength()];
            data[0] = (byte)(((short)status >> 8) & 0xFF);
            data[1] = (byte)((short)status & 0xFF);
            Array.Copy(reason.GetBytes(), 0, data, 2, reason.ByteLength());

            return new Frame {
                Opcode = kOpcode.Close,
                Content = data
            };
        }

        public static Frame Ping(string data = "") {
            return new Frame {
                Opcode = kOpcode.Ping,
                Content = data.GetBytes()
            };
        }

        public static Frame Pong(string data = "") {
            return new Frame {
                Opcode = kOpcode.Pong,
                Content = data.GetBytes()
            };
        }

        public static Frame Pong(Frame pingFrame) {
            pingFrame.Opcode = kOpcode.Pong;
            return pingFrame;
        }
    }
}
