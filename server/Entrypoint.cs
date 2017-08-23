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
    class Entrypoint {
        static void Main(string[] args) {
            DbConfiguration.SetConfiguration(new MySqlEFConfiguration());
            var db = new DAL.ScapeDb();
            var a = db.Users.ToList();

            Dictionary<int, Server> servers
                = new Dictionary<int, Server>();
            Dictionary<int, Pool<PlayerConnection>> pools
                = new Dictionary<int, Pool<PlayerConnection>>();

            foreach(var server in Configuration.Servers) {
                var pool = new Pool<PlayerConnection> {
                    InitialCount = 3,
                    InitialSize = 3,
                    SizeGrowth = 3,
                    MaxSize = 100,
                    MaxTotal = server["Max Users"] ?? Configuration.General["Max Users"]
                };

                pools.Add(server["Id"], pool);
                servers.Add(server["Id"], new Server<PlayerConnection>((ushort)server["Port"], pool, server));
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
