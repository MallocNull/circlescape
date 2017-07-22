using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SockScape.Socks {
    static class MasterUdpServer {
        private static UdpClient Sock;
        private static Thread ListeningThread = null;
        private static bool IsOpen = false;

        public static void Initialize() {
            if(!IsOpen && ListeningThread == null)
                return;

            short port = (short)Configuration.General["Master Port"];
            Sock = new UdpClient(port);
            
            IsOpen = true;
            ListeningThread = new Thread(new ThreadStart(Listener));
            ListeningThread.Start();
        }

        public static void Listener() {
            while(IsOpen) {
                while(Sock.Available > 0) {

                }

                Thread.Sleep(1);
            }
        }

        public static void Close() {
            IsOpen = false;
            ListeningThread.Join();
            ListeningThread = null;
        }
    }
}
