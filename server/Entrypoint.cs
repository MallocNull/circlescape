using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using CircleScape.DAL;
using System.Numerics;
using Glove;
using System.Net;
using Kneesocks;
using MySql.Data.Entity;

namespace SockScape {
    static class ServerContext {
        public static Dictionary<int, Server<PlayerConnection>> Servers { get; }
            = new Dictionary<int, Server<PlayerConnection>>();
        public static Dictionary<int, Pool<PlayerConnection>> Pools { get; }
            = new Dictionary<int, Pool<PlayerConnection>>();
    }

    class Entrypoint {
        static void Main(string[] args) {
            foreach(var server in Configuration.Servers) {
                var pool = new Pool<PlayerConnection> {
                    InitialCount = 3,
                    InitialSize = 3,
                    SizeGrowth = 3,
                    MaxSize = 100,
                    MaxTotal = server["Max Users"] ?? Configuration.General["Max Users"]
                };

                var serverHandle = new Server<PlayerConnection>((ushort)server["Port"], pool, server);

                ServerContext.Pools.Add(server["Id"], pool);
                ServerContext.Servers.Add(server["Id"], serverHandle);
                serverHandle.Start();
            }

            //var server = new Server<PlayerConnection>(6770, PoolManager.Pending);
            //server.Start();

            /*while(true) {
                var send = Console.ReadLine();
                PoolManager.Pending.Broadcast(Encoding.UTF8.GetBytes(send));

                // logic processing loop
            }*/

            Console.ReadLine();

            //server.Stop();
        }
    }
}
