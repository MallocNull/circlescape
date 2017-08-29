using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SockScape {
    static class MasterUdpServer {
        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;

            short port = (short)Configuration.General["Master Port"];
            Sock = new UdpClient(new IPEndPoint(IPAddress.Any, port));
            
            IsOpen = true;
            ListeningThread = new Thread(Listener);
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
            Sock.Dispose();
        }
    }
}
