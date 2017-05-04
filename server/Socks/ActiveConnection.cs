using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape {
    class ActiveConnection : Kneesocks.Connection {
        public ActiveConnection(UInt32 id, TcpClient sock) : base(id, sock) { }
        public ActiveConnection(PendingConnection conn) : base(conn) { }
    }
}
