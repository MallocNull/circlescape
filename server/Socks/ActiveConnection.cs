using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Kneesocks;
using CircleScape.Encryption;

namespace CircleScape {
    class ActiveConnection : Connection {
        private Cipher Encryptor;

        public void Initialize(PendingConnection conn) {
            Initialize(conn, false);
            Encryptor = conn.Encryptor;
        }
    }
}
