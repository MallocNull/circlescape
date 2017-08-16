using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net.Sockets;
using System.Net;

namespace Kneesocks {
    public abstract class Server {
        protected TcpListener Socket;
        protected Thread Listener = null;
        public bool Started { get; protected set; } = false;
        public UInt16 Port { get; protected set; }
        public object Configuration { get; protected set; }

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
    }

    public class Server<T> : Server where T : Connection, new() {
        protected Pool<T> ConnectionPool = null;

        public Server(UInt16 port, Pool<T> pool, object config = null) {
            Port = port;
            Socket = new TcpListener(IPAddress.Any, port);
            Listener = new Thread(new ThreadStart(ListenThread));
            ConnectionPool = pool;
            Configuration = config;
        }

        protected void ListenThread() {
            Socket.Start();
            
            while(Started) {
                if(Socket.Pending()) {
                    var templatedConnection = new T() {
                        Server = this
                    };
                    templatedConnection.Initialize(Socket.AcceptTcpClient());

                    if(!ConnectionPool.AddConnection(templatedConnection))
                        templatedConnection.Disconnect("Connection pooler rejected connection.");
                }

                Thread.Sleep(100);
            }

            Socket.Stop();
        }
    }
}

