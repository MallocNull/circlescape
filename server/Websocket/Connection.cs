using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.Websocket {
    class Connection {
        private TcpClient Socket;

        public Connection(TcpClient sock) {
            Socket = sock;
        }


    }
}
