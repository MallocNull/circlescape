using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Glove;
using SockScape.Encryption;

namespace SockScape {
    static class MasterUdpClient {
        private static Key Key;
        public static Cipher Encryptor { get; private set; }

        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;

            short port = (short)Configuration.General["Master Port"];
            Sock = new UdpClient(Configuration.General["Master Addr"], port);

            Key = new Key();

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
        }
    }
}
