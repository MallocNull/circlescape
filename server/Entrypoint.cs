using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CircleScape.DAL;

namespace CircleScape {
    class Entrypoint {
        static void Main(string[] args) {
            var server = new Kneesocks.Server<PendingConnection>(6770, PoolManager.Pool);
            server.Start();

            while(true) {
                // physics and game logic processing loop
            }
        }
    }
}
