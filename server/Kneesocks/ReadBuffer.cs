using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Kneesocks {
    class ReadBuffer {
        private List<byte> Buffer;
        private int ExpectedLength;
        private string ExpectedString;
        private NetworkStream Source;
        private DateTime StartTime;

        public bool IsReading { get; private set; } = false;

        public ReadBuffer(NetworkStream source) {
            Source = source;
        }

        public TimeSpan ElapsedReadTime {
            get {
                return DateTime.Now - StartTime;
            }
        }

        private byte[] CheckBuffer() {
            byte[] returnValue = null;

            if(ExpectedString != null) {
                var location = Encoding.ASCII.GetString(Buffer.ToArray()).IndexOf(ExpectedString);
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

            byte[] returnValue;
            if((returnValue = CheckBuffer()) != null)
                return returnValue;
            
            var buffer = new byte[1024];
            while(Source.DataAvailable) {
                var readAmount = ExpectedString == null
                        ? Math.Min(1024, ExpectedLength - Buffer.Count)
                        : 1024;

                var bytesRead = Source.Read(buffer, 0, readAmount);
                if(bytesRead == readAmount)
                    Buffer.AddRange(buffer);
                else
                    Buffer.AddRange(buffer.Take(readAmount));

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
            StartTime = DateTime.Now;

            return AttemptRead();
        }

        public byte[] AttemptRead(string terminator) {
            if(IsReading)
                return null;

            IsReading = true;
            ExpectedString = terminator;
            StartTime = DateTime.Now;

            return AttemptRead();
        }
    }
}
