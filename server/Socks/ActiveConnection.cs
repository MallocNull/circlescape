using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Kneesocks;

namespace CircleScape {
    class ActiveConnection : Connection {
        public ActiveConnection(TcpClient sock) : base(sock) { }

        public ActiveConnection(PendingConnection conn) : base(conn) { }
    }
}
