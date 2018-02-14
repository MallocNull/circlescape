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
        public static Dictionary<ushort, Server<PlayerConnection>> Servers { get; }
            = new Dictionary<ushort, Server<PlayerConnection>>();
        public static Dictionary<ushort, Pool<PlayerConnection>> Pools { get; }
            = new Dictionary<ushort, Pool<PlayerConnection>>();

        public static Packet StatusUpdatePacket {
            get {
                var packet = new Packet(kIntraSlaveId.StatusUpdate, (byte)Servers.Count);
                foreach(var pool in Pools)
                    packet.AddRegions(pool.Key.Pack(), ((ushort)pool.Value.ConnectionCount).Pack(), Servers[pool.Key].Port.Pack());

                return packet;
            }
        }
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

                ServerContext.Pools.Add((ushort)server["Id"], pool);
                ServerContext.Servers.Add((ushort)server["Id"], serverHandle);
                serverHandle.Start();
            }

            if(Configuration.General["Run Master"])
                MasterIntraServer.Initialize();

            MasterIntraClient.Initialize();

            /*while(true) {
                var send = Console.ReadLine();
                PoolManager.Pending.Broadcast(Encoding.UTF8.GetBytes(send));

                // logic processing loop
            }*/

            Console.ReadLine();

            MasterIntraClient.Close();
            if(Configuration.General["Run Master"])
                MasterIntraServer.Close();
        }
    }
}
