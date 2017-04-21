using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace CircleScape.Websocket {
    class Stack {
        private List<Connection> Clients = new List<Connection>();
        private Mutex ClientsMutex = new Mutex();
        private bool RunWithNoClients = false;

        public Stack(Connection initialConnection = null) {
            if(initialConnection != null)
                Clients.Add(initialConnection);
        }

        public Stack(bool runWithNoClients, Connection initialConnection = null)
            : this(initialConnection) 
        {
            RunWithNoClients = runWithNoClients;
        }

        public bool AddClient(Connection client) {
            if(!ClientsMutex.WaitOne(5000))
                return false;

            Clients.Add(client);

            ClientsMutex.ReleaseMutex();
            return true;
        }

        public int ClientCount {
            get {
                return Clients.Count;
            }
        }

        // USED FOR THREADING -- DO NOT CALL
        public void ManageStack() {
            int clientCount = ClientCount;

            for(var i = 0; i < clientCount; ++i)
        }
    }
}
