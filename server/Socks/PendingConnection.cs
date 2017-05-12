using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Square;
using Kneesocks;

namespace CircleScape {
    class PendingConnection : Connection {
        private DateTime ConnectionOpened;

        protected override void OnOpen() {
            ConnectionOpened = DateTime.UtcNow;
        }

        protected override void OnParse() {
            if((DateTime.UtcNow - ConnectionOpened).Seconds > 60) {
                Disconnect(Frame.kClosingReason.ProtocolError, "Logon request timed out.");
            }
        }

        protected override void OnReceive(byte[] data) {
            Console.WriteLine(Id + " says " + data.GetString());
        }
    }
}
