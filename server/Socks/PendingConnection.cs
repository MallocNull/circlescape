using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape {
    class PendingConnection : ActiveConnection {
        public PendingConnection(TcpClient sock) : base(sock) { }
    }
}
