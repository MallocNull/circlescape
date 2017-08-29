using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SockScape {
    class MasterUdpClient {
        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen)
                return;

            short port = (short) Configuration.General["Master Port"];
            Sock = new UdpClient(Configuration.General["Master Addr"], port);

            IsOpen = true;
        }

        public static void Listener() {
            
        }

        public static void Close() {
            IsOpen = false;
            ListeningThread.Join();
            ListeningThread = null;
        }
    }
}
