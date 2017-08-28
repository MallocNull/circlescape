using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace SockScape.Socks {
    class MasterUdpClient {
        private static UdpClient Sock;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen)
                return;

            short port = (short) Configuration.General["Master Port"];
            Sock = new UdpClient(port);

            IsOpen = true;
        }
    }
}
