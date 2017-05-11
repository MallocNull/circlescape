using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace Kneesocks {
    public class Stack<T> where T : Connection {
        private Pool<T> PoolRef = null;
        private List<T> Clients = new List<T>();
        private bool RunWithNoClients = false;
        private bool Running = true;

        public Stack(Pool<T> poolRef, T initialConnection = null) {
            PoolRef = poolRef;
            if(initialConnection != null)
                Clients.Add(initialConnection);
        }

        public Stack(Pool<T> poolRef, bool runWithNoClients, T initialConnection = null)
            : this(poolRef, initialConnection) 
        {
            RunWithNoClients = runWithNoClients;
        }

        public void AddClient(T client) {
            lock(Clients) {
                Clients.Add(client);
            }
        }

        public int Count {
            get {
                return Clients.Count;
            }
        }

        public void StopThread() {
            Running = false;
        }

        public bool Finished { get; private set; }

        public bool UnlistConnection(Connection connection) {
            lock(Clients) {
                foreach(var conn in Clients) {

                }
            }
        }

        // USED FOR THREADING -- DO NOT CALL
        public void ManageStack() {
            while(Running && (Count > 0 || RunWithNoClients)) {
                lock(Clients) {
                    for(var i = Count - 1; i >= 0 && Running; --i) {
                        var client = Clients[i];
                        var connected = !client.Disconnected;

                        if(connected) {
                            try {
                                client.Parse();
                            } catch {
                                connected = false;
                            }
                        }

                        if(!connected) {
                            PoolRef.InvalidateConnection(client.Id);
                            Clients.RemoveAt(i);
                        }
                    }
                }

                Thread.Sleep(10);
            }

            Finished = true;
            PoolRef.InvalidateThread(this);
        }
    }
}
