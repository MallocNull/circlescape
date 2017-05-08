using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Kneesocks {
    public class Connection {
        public UInt64? _Id = null;
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

        private TcpClient Socket;
        private NetworkStream Stream;

        ReadBuffer Buffer;
        private Frame PartialFrame = null;
        private List<Frame> FrameBuffer = new List<Frame>();

        public bool Disconnected { get; private set; } = false;
        public string DisconnectReason { get; private set; } = null;

        public bool Handshaked { get; private set; } = false;
        public Handshake ClientHandshake { get; private set; } = null;

        public Connection(TcpClient sock) {
            Socket = sock;
            Socket.ReceiveTimeout = 1;
            Stream = sock.GetStream();
            Buffer = new ReadBuffer(Stream);
        }

        public Connection(UInt64 id, TcpClient sock) : this(sock) {
            Id = id;
        }

        public Connection(Connection conn, bool preserveId = true) {
            if(preserveId)
                _Id = conn._Id;

            Socket = conn.Socket;
            Stream = conn.Stream;

            Buffer = conn.Buffer;
            PartialFrame = conn.PartialFrame;
            FrameBuffer = conn.FrameBuffer;

            Disconnected = conn.Disconnected;
            DisconnectReason = conn.DisconnectReason;

            Handshaked = conn.Handshaked;
            ClientHandshake = conn.ClientHandshake;
        }

        public byte[] Parse() {
            byte[] readBuffer = null;
            if(Buffer.IsReading) {
                readBuffer = Buffer.AttemptRead();
                if(readBuffer == null) {
                    if(Buffer.ElapsedReadTime.Seconds > 30)
                        Disconnect(Frame.kClosingReason.ProtocolError, "Timed out waiting for a full response");

                    return null;
                }
            }
            
            if(!Handshaked) {
                if(!Buffer.IsReading) {
                    readBuffer = Buffer.AttemptRead("\r\n\r\n");
                    if(readBuffer == null)
                        return null;
                }
                
                try {
                    Handshake request = new Handshake(Encoding.ASCII.GetString(readBuffer));
                    var response = Handshake.AcceptRequest(request).ToBytes();
                    Stream.Write(response, 0, response.Length);
                    ClientHandshake = request;
                    Handshaked = true;
                } catch(Exception e) {
                    Disconnect(Frame.kClosingReason.ProtocolError, e.Message);
                    return null;
                }

                OnOpen();
                return null;
            }

            /*if(!Buffer.IsReading) {
                readBuffer = Buffer.AttemptRead("\r\n\r\n");
                if(readBuffer == null)
                    return null;
            }*/

            OnParse();
            return null;
        }

        public void Disconnect(string reason = null) {
            Disconnect(Frame.kClosingReason.Normal, reason);
        }

        public void Disconnect(Frame.kClosingReason status, string reason = null) {
            Disconnected = true;
            DisconnectReason = reason;

            if(Socket.Connected) {
                Socket.SendTimeout = 1000;
                var raw = Handshaked ? Frame.Closing(status, reason).GetBytes()
                                     : Handshake.DenyRequest().ToString().GetBytes();
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
