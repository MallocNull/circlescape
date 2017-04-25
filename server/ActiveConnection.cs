using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape {
    class ActiveConnection : Websocket.Connection {
        public ActiveConnection(TcpClient sock) : base(sock) { }

        public ActiveConnection(PendingConnection conn) : base(conn) { }
    }
}
