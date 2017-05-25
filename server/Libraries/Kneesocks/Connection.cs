using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Square;
using System.IO;

namespace Kneesocks {
    public class Connection {
        private bool Initialized = false;

        private UInt64? _Id = null;
        public UInt64 Id {
            get {
                if(_Id == null)
                    throw new ArgumentNullException();
                else
                    return (UInt64)_Id;
            }
            set {
                if(_Id == null)
                    _Id = value;
            }
        }
        internal bool IsIdNull {
            get => _Id == null;
        }

        private TcpClient Socket = null;
        private NetworkStream Stream = null;

        ReadBuffer Buffer;
        private byte[] FirstTwoBytes = null;
        private int ExtraHeaderSize = 0;
        private byte[] FrameHeader = null;
        private List<Frame> ReceiveFrameBuffer = new List<Frame>();
        private List<Frame> SendFrameBuffer = new List<Frame>();
        private const int MaximumSendFrameSize = 0xFFFFF;

        protected const int PingInterval    = 30;
        protected const int TimeoutInterval = 120;
        private byte[] PingData = Encoding.ASCII.GetBytes("woomy!");
        private DateTime LastPing;
        private bool AwaitingPingResponse = false;
        private TimeSpan TimeSinceLastPing {
            get => DateTime.UtcNow - LastPing;
        }

        internal bool OutsidePool = false;
        public bool Disconnected { get; private set; } = false;
        public string DisconnectReason { get; private set; } = null;

        public bool Handshaked { get; private set; } = false;
        public Handshake ClientHandshake { get; private set; } = null;

        public void Initialize(TcpClient sock) {
            if(Initialized)
                return;

            Socket = sock;
            Socket.ReceiveTimeout = 1;
            Stream = sock.GetStream();
            Buffer = new ReadBuffer(Stream);

            Initialized = true;
        }

        public void Initialize(UInt64 id, TcpClient sock) {
            if(Initialized)
                return;

            Initialize(sock);
            Id = id;

            Initialized = true;
        }

        public void Initialize(Connection conn, bool preserveId = false) {
            if(Initialized)
                return;

            if(preserveId)
                _Id = conn._Id;

            Socket = conn.Socket;
            Stream = conn.Stream;

            Buffer = conn.Buffer;
            FirstTwoBytes = conn.FirstTwoBytes;
            ExtraHeaderSize = conn.ExtraHeaderSize;
            FrameHeader = conn.FrameHeader;
            ReceiveFrameBuffer = conn.ReceiveFrameBuffer;

            LastPing = conn.LastPing;

            Disconnected = conn.Disconnected;
            DisconnectReason = conn.DisconnectReason;

            Handshaked = conn.Handshaked;
            ClientHandshake = conn.ClientHandshake;

            Initialized = true;
        }

        private void _Send(byte[] message, bool isFinal = true, bool singleFrame = false, bool first = false) {
            int frameCount = singleFrame ? 0 : (message.Length / MaximumSendFrameSize);
            for(var i = 0; i <= frameCount; ++i) {
                SendFrameBuffer.Add(new Frame {
                    IsFinal = (i == frameCount && isFinal) ? true : false,
                    IsMasked = false,
                    Opcode = (i == 0 || (singleFrame && first)) ? Frame.kOpcode.BinaryFrame : Frame.kOpcode.Continuation,
                    Content = message.Subset(i * (MaximumSendFrameSize + 1), MaximumSendFrameSize)
                });
            }
        }

        public void Send(byte[] message) {
            lock(SendFrameBuffer) {
                _Send(message);
            }
        }
        
        private void _Send(Stream stream, bool startingFrame = true) {
            if(!Socket.Connected)
                return;

            bool firstRead = true;
            byte[] byteBuffer = new byte[MaximumSendFrameSize];
            while(true) {
                var bytesRead = stream.Read(byteBuffer, 0, MaximumSendFrameSize);

                if(stream.Position == stream.Length) {
                    _Send(bytesRead == MaximumSendFrameSize ? byteBuffer : byteBuffer.Take(bytesRead).ToArray(), true, true, firstRead);
                    return;
                } else {
                    _Send(bytesRead == MaximumSendFrameSize ? byteBuffer : byteBuffer.Take(bytesRead).ToArray(), false, true, firstRead);
                }

                firstRead = false;
            }
        }

        public void Send(Stream stream) {
            lock(SendFrameBuffer) {
                _Send(stream);
            }
        }

        public void Send(byte[] preamble, Stream stream) {
            lock(SendFrameBuffer) {
                _Send(preamble, false);
                _Send(stream, false);
            }
        }
        
        private void ReadIfNotNull(ref byte[] buffer, int length) {
            buffer = buffer ?? Buffer.AttemptRead(length)
;
        }

        private void ReadIfNotNull(ref byte[] buffer, string terminator) {
            buffer = buffer ?? Buffer.AttemptRead(terminator)
;
        }
        
        internal void Parse() {
            if(Handshaked) {
                if(!Buffer.IsReading) {
                    if(TimeSinceLastPing.Seconds > TimeoutInterval) {
                        Disconnect(Frame.kClosingReason.Normal, "Ping response timed out.");
                    } else if(TimeSinceLastPing.Seconds > PingInterval && !AwaitingPingResponse) {
                        var frameBytes = new Frame {
                            IsFinal = true,
                            IsMasked = false,
                            Opcode = Frame.kOpcode.Ping,
                            Content = PingData
                        }.GetBytes();

                        Stream.Write(frameBytes, 0, frameBytes.Length);
                        AwaitingPingResponse = true;
                    }
                }

                lock(SendFrameBuffer) {
                    if(SendFrameBuffer.Count > 0) {
                        foreach(var frame in SendFrameBuffer) {
                            var frameBytes = frame.GetBytes();
                            Stream.Write(frameBytes, 0, frameBytes.Length);
                        }

                        SendFrameBuffer = new List<Frame>();
                    }
                }
            }

            OnParse();

            byte[] readBuffer = null;
            if(Buffer.IsReading) {
                readBuffer = Buffer.AttemptRead();
                if(readBuffer == null) {
                    if((!Handshaked || (Handshaked && FirstTwoBytes != null)) && Buffer.ElapsedReadTime.Seconds > (Handshaked ? 300 : 30))
                        Disconnect(Frame.kClosingReason.ProtocolError, "Timed out waiting for a full response");

                    return;
                }
            }
            
            if(!Handshaked) {
                ReadIfNotNull(ref readBuffer, "\r\n\r\n");
                if(readBuffer == null)
                    return;

                try {
                    Handshake request = new Handshake(Encoding.ASCII.GetString(readBuffer));
                    var response = Handshake.AcceptRequest(request).ToBytes();
                    Stream.Write(response, 0, response.Length);
                    ClientHandshake = request;
                    Handshaked = true;

                    LastPing = DateTime.UtcNow;
                } catch(Exception e) {
                    Disconnect(Frame.kClosingReason.ProtocolError, e.Message);
                    return;
                }

                OnOpen();
                return;
            }

            if(FirstTwoBytes == null) {
                ReadIfNotNull(ref readBuffer, 2);
                if(readBuffer == null)
                    return;

                FirstTwoBytes = readBuffer;
                ExtraHeaderSize = Frame.HeaderLengthFromBytes(FirstTwoBytes) - 2;

                readBuffer = null;
            }

            if(FrameHeader == null) {
                if(ExtraHeaderSize == 0)
                    FrameHeader = FirstTwoBytes;
                else {
                    ReadIfNotNull(ref readBuffer, ExtraHeaderSize);
                    if(readBuffer == null)
                        return;

                    FrameHeader = FirstTwoBytes.Concat(readBuffer).ToArray();
                }

                readBuffer = null;
            }

            if(FrameHeader != null) {
                Frame tempFrame;

                if(readBuffer == null) {
                    try {
                        tempFrame = Frame.HeaderFromBytes(FrameHeader);
                    } catch(Exception e) {
                        Disconnect(Frame.kClosingReason.ProtocolError, e.Message);
                        return;
                    }

                    readBuffer = Buffer.AttemptRead(tempFrame.BodyLength);
                    if(readBuffer == null)
                        return;
                }

                try {
                    tempFrame = Frame.FromBytes(FrameHeader.Concat(readBuffer).ToArray());
                } catch(Exception e) {
                    Disconnect(Frame.kClosingReason.ProtocolError, e.Message);
                    return;
                }

                ReceiveFrameBuffer.Add(tempFrame);
                FirstTwoBytes = null;
                ExtraHeaderSize = 0;
                FrameHeader = null;

                if(tempFrame.IsFinal) {
                    switch(tempFrame.Opcode) {
                        case Frame.kOpcode.Ping:
                            LastPing = DateTime.UtcNow;
                            AwaitingPingResponse = false;

                            tempFrame.Opcode = Frame.kOpcode.Pong;
                            var pingBuffer = tempFrame.GetBytes();
                            Stream.Write(pingBuffer, 0, pingBuffer.Length);
                            break;

                        case Frame.kOpcode.Pong:
                            LastPing = DateTime.UtcNow;
                            AwaitingPingResponse = false;
                            break;

                        case Frame.kOpcode.Close:
                            Disconnect(Frame.kClosingReason.Normal, "Connection closed.");
                            break;

                        case Frame.kOpcode.BinaryFrame:
                        case Frame.kOpcode.TextFrame:
                        case Frame.kOpcode.Continuation:
                            byte[] byteBuffer = new byte[0];
                            foreach(var frame in ReceiveFrameBuffer)
                                byteBuffer = byteBuffer.Concat(frame.Content).ToArray();

                            ReceiveFrameBuffer = new List<Frame>();
                            OnReceive(byteBuffer);
                            break;
                    }
                }
            }
        }

        public void RemoveFromPool() => OutsidePool = true;

        public void Disconnect(string reason = null) => Disconnect(Frame.kClosingReason.Normal, reason);

        public void Disconnect(Frame.kClosingReason status, string reason = null) {
            Disconnected = true;
            DisconnectReason = reason;

            if(Socket.Connected) {
                Socket.SendTimeout = 1000;
                var raw = Handshaked ? Frame.Closing(status, reason).GetBytes()
                                     : Handshake.DenyRequest(message: reason).ToString().GetBytes();
                Stream.Write(raw, 0, raw.Length);
                Socket.Close();
            }

            OnClose();
        }

        // called after the client successfully handshakes
        protected virtual void OnOpen() { }

        // called when the thread manager iterates through
        // the thread list and stops on this thread
        protected virtual void OnParse() { }

        // called when data has been received
        protected virtual void OnReceive(byte[] data) { }

        // called when the connection is disconnected
        protected virtual void OnClose() { }
    }
}
