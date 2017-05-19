using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CircleScape.DAL;
using System.Numerics;
using Square;

namespace CircleScape {
    class Entrypoint {
        static void Main(string[] args) {
            var server = new Kneesocks.Server<PendingConnection>(6770, PoolManager.Pending);
            server.Start();

            while(true) {
                var send = Console.ReadLine();
                PoolManager.Pending.Broadcast(Encoding.UTF8.GetBytes(send));

                // logic processing loop
            }

            server.Stop();
            PoolManager.Dispose();
        }
    }
}
