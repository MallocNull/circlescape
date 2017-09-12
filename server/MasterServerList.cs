using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.Remoting;
using System.Text;
using System.Threading.Tasks;
using Glove;

namespace SockScape {
    static class MasterServerList {
        private static Dictionary<UInt16, Server> _Servers
            = new Dictionary<UInt16, Server>();

        public static Dictionary<UInt16, Server> Servers {
            get {
                lock(_Servers) {
                    return _Servers.ToDictionary(x => x.Key,
                                                 x => x.Value);
                }
            }
        }

        public static void Write(Server server) {
            lock(_Servers) {
                if(HasId(server.Id) && !_Servers[server.Id].Address.Equals(server.Address))
                    Console.WriteLine($"{DateTime.Now.ToShortTimeString()} - Server {server.Id} has changed IP addresses.");

                _Servers[server.Id] = server;
            }
        }

        public static Packet ReportPacket {
            get {
                lock(_Servers) {
                    var packet = new Packet(kInterMasterId.ServerListing, ((ushort)_Servers.Count).Pack());
                    foreach(var server in _Servers)
                        // TODO change this to support IPv6
                        packet.AddRegions(server.Key.Pack(), server.Value.UserCount.Pack());

                    return packet;
                }
            }
        }

        public static void RemoveServersByOwners(IEnumerable<MasterIntraServer.Client> owners) {
            lock(_Servers) {
                _Servers = _Servers.Where(x => !owners.Contains(x.Value.Owner))
                                   .ToDictionary(x => x.Key, x => x.Value);
            }
        }

        public static bool HasId(UInt16 id) {
            lock(_Servers) {
                return _Servers.ContainsKey(id);
            }   
        }

        public static void Clear() {
            lock(_Servers) {
                _Servers.Clear();
            }
        } 
    }

    class Server {
        public ushort Id { get; set; }
        public ushort UserCount { get; set; }
        public IPAddress Address { get; set; }
        public ushort Port { get; set; }
        public MasterIntraServer.Client Owner { get; set; }
    }
}
