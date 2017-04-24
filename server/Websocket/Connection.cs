using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.Websocket {
    abstract class Connection {
        private TcpClient Socket;
        private NetworkStream Stream;

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

        public void Disconnect(string reason = null) {
            Disconnected = true;
            DisconnectReason = reason;
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
