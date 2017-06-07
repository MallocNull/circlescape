using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using CircleScape.DAL;
using System.Numerics;
using Square;
using System.Net;

namespace CircleScape {
    class Entrypoint {
        static void Main(string[] args) {
            foreach()
            var server = new Kneesocks.Server<PlayerConnection>(6770, PoolManager.Pending);
            server.Start();

            /*while(true) {
                var send = Console.ReadLine();
                PoolManager.Pending.Broadcast(Encoding.UTF8.GetBytes(send));

                // logic processing loop
            }*/

            Console.ReadLine();

            server.Stop();
            PoolManager.Dispose();
        }
    }
}
