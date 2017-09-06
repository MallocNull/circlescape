using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace SockScape {
    static class ServerList {
        private static ServerDictionary Servers { get; } 
            = new ServerDictionary();

        private static Dictionary<UInt16, Server> List
            => Servers.List;

        public static void Write(Server server) {
            lock(Servers) {
                if(HasId(server.Id) && !List[server.Id].Address.Equals(server.Address))
                    Console.WriteLine($"{DateTime.Now.ToShortTimeString()} - Server {server.Id} has changed IP addresses.");

                List[server.Id] = server;
            }
        }

        public static Packet ReportPacket {
            get {
                var packet = new Packet(kInterMasterId.);

                return packet;
            }
        }

        public static bool HasId(UInt16 id)
            => List.ContainsKey(id);

        public static void Clear()
            => List.Clear();

        public class ServerDictionary {
            public Dictionary<UInt16, Server> List { get; }
                = new Dictionary<UInt16, Server>();

            public Server this[UInt16 i] {
                get => List.ContainsKey(i) ? List[i] : null;
                set => List[i] = value;
            }
        }
    }

    class Server {
        public ushort Id { get; set; }
        public ushort UserCount { get; set; }
        public IPAddress Address { get; set; }
        public ushort Port { get; set; }
    }
}
