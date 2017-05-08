using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;

namespace Kneesocks {
    public class Server<T> where T : Connection {
        private TcpListener Socket;
        private Thread Listener = null;
        private Pool<T> ConnectionPool = null;
        private bool Started = false;
        public UInt16 Port { get; private set; }

        public Server(UInt16 port, Pool<T> pool) {
            Port = port;
            Socket = new TcpListener(IPAddress.Any, port);
            Listener = new Thread(new ThreadStart(ListenThread));
            ConnectionPool = pool;
        }

        public void Start() {
            if(!Started) {
                Listener.Start();
                Started = true;
            }
        }

        public void Stop() {
            if(Started) {
                Started = false;
                Listener.Join();
            }
        }

        private void ListenThread() {
            Socket.Start();
            
            while(Started) {
                if(Socket.Pending())
                    ConnectionPool.AddConnection(new Connection(Socket.AcceptTcpClient()));

                Thread.Sleep(100);
            }

            Socket.Stop();
        }
    }
}

