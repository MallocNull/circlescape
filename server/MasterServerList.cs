using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Glove;

namespace SockScape {
    static class MasterServerList {
        public static Dictionary<UInt16, Server> Servers { get; }
            = new Dictionary<UInt16, Server>();

        public static void Write(Server server) {
            lock(Servers) {
                if(HasId(server.Id) && !Servers[server.Id].Address.Equals(server.Address))
                    Console.WriteLine($"{DateTime.Now.ToShortTimeString()} - Server {server.Id} has changed IP addresses.");

                Servers[server.Id] = server;
            }
        }

        public static Packet ReportPacket {
            get {
                lock(Servers) {
                    var packet = new Packet(kInterMasterId.ServerListing, ((ushort)Servers.Count).Pack());
                    foreach(var server in Servers)
                        // TODO change this to support IPv6
                        packet.AddRegions(server.Key.Pack(), server.Value.UserCount.Pack(), 
                            server.Value.Address.MapToIPv4().ToString(), server.Value.Port.Pack());

                    return packet;
                }
            }
        }

        public static bool HasId(UInt16 id)
            => Servers.ContainsKey(id);

        public static void Clear()
            => Servers.Clear();
    }

    class Server {
        public ushort Id { get; set; }
        public ushort UserCount { get; set; }
        public IPAddress Address { get; set; }
        public ushort Port { get; set; }
    }
}
