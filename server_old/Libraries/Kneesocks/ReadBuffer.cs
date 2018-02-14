using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Kneesocks {
    internal class ReadBuffer {
        private const int BufferSize = 1024;

        private List<byte> Buffer;
        private int ExpectedLength;
        private string ExpectedString;
        private readonly NetworkStream Source;
        private DateTime StartTime;

        public bool IsReading { get; private set; }

        public ReadBuffer(NetworkStream source) {
            Source = source;
            Buffer = new List<byte>();
        }

        public TimeSpan ElapsedReadTime
            => DateTime.UtcNow - StartTime;

        private byte[] CheckBuffer() {
            byte[] returnValue = null;

            if(ExpectedString != null) {
                var location = Encoding.ASCII.GetString(Buffer.ToArray()).IndexOf(ExpectedString, StringComparison.InvariantCulture);
                if(location != -1) {
                    var fullJump = location + ExpectedString.Length;
                    returnValue = Buffer.Take(fullJump).ToArray();
                    Buffer = Buffer.Skip(fullJump).ToList();
                    IsReading = false;
                }
            } else {
                if(Buffer.Count >= ExpectedLength) {
                    returnValue = Buffer.Take(ExpectedLength).ToArray();
                    Buffer = Buffer.Skip(ExpectedLength).ToList();
                    IsReading = false;
                }
            }

            return returnValue;
        }

        public byte[] AttemptRead() {
            if(!IsReading)
                return null;

            if(!Source.CanRead)
                return null;

            byte[] returnValue;
            if((returnValue = CheckBuffer()) != null)
                return returnValue;
            
            var buffer = new byte[BufferSize];
            while(Source.DataAvailable) {
                var readAmount = ExpectedString == null
                        ? Math.Min(BufferSize, ExpectedLength - Buffer.Count)
                        : BufferSize;

                var bytesRead = Source.Read(buffer, 0, readAmount);
                Buffer.AddRange(bytesRead == BufferSize ? buffer : buffer.Take(bytesRead));

                if((returnValue = CheckBuffer()) != null)
                    return returnValue;
            }

            return null;
        }

        public byte[] AttemptRead(int length) {
            if(IsReading)
                return null;

            IsReading = true;
            ExpectedLength = length;
            ExpectedString = null;
            StartTime = DateTime.UtcNow;

            return AttemptRead();
        }

        public byte[] AttemptRead(string terminator) {
            if(IsReading)
                return null;

            IsReading = true;
            ExpectedString = terminator;
            StartTime = DateTime.UtcNow;

            return AttemptRead();
        }
    }
}
