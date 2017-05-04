using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape {
    class PendingConnection : Kneesocks.Connection {
        public PendingConnection(UInt32 id, TcpClient sock) : base(id, sock) { }
        
    }
}
