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
        private string RawBuffer = "";
        private List<Frame> FrameBuffer = new List<Frame>();

        public bool Disconnected { get; private set; } = false;
        public string DisconnectReason { get; private set; } = null;

        public bool Handshaked { get; private set; } = false;
        private string RawClientHandshake = "";
        private Dictionary<string, string> Headers =
            new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

        public Connection(TcpClient sock) {
            Socket = sock;
            Socket.ReceiveTimeout = 1;
            Stream = sock.GetStream();
        }

        public Connection(UInt64 id, TcpClient sock) : this(sock) {
            Id = id;
        }

        public Connection(Connection conn) {
            Id = conn.Id;
            Socket = conn.Socket;
            Stream = Socket.GetStream();

            Disconnected = conn.Disconnected;
            DisconnectReason = conn.DisconnectReason;

            Handshaked = conn.Handshaked;
            RawClientHandshake = conn.RawClientHandshake;
            Headers = conn.Headers;
        }

        public byte[] Parse() {
            if(!Handshaked) {

            } else {

            }
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
        public virtual void OnOpen() { }

        // called when the thread manager iterates through
        // the thread list and stops on this thread
        public virtual void OnParse() { }

        // called when data has been received
        public virtual void OnReceive(byte[] data) { }

        // called when the connection is disconnected
        public virtual void OnClose() { }
    }
}
