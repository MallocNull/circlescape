using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace SockScape {
    class ServerList {
        public static Dictionary<int, Server> Servers { get; } 
            = new Dictionary<int, Server>();

        public Server this[int i] {
            get => Servers.ContainsKey(i) ? Servers[i] : null;
            set => Servers[i] = value;
        }

        public bool HasId(int id)
            => Servers.ContainsKey(id);

        public void Clear()
            => Servers.Clear();
    }

    class Server {
        public ushort Id { get; set; }
        public ushort UserCount { get; set; }
        public IPEndPoint Address { get; set; }
        public IPEndPoint Owner { get; set; }
    }
}
