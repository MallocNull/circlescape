using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Kneesocks;
using Glove;
using SockScape.Encryption;

namespace SockScape {
    class PlayerConnection : Connection {
        private DateTime ConnectionOpened;

        protected override void OnOpen() {
            ConnectionOpened = DateTime.UtcNow;
        }

        protected override void OnParse() {
            if((DateTime.UtcNow - ConnectionOpened).TotalSeconds > 60) {
                Disconnect(Frame.kClosingReason.ProtocolError, "Logon request timed out.");
            }
        }

        protected override void OnReceive(byte[] data) {
            Packet packet = Packet.FromBytes(data);

            if(packet == null) {
                Disconnect(Frame.kClosingReason.ProtocolError, "Packet received was not legal.");
                return;
            }

            switch((kClientServerId)packet.Id) {
                default:
                    Disconnect(Frame.kClosingReason.ProtocolError, "Packet ID could not be understood at this time.");
                    break;
            }

            Console.WriteLine(Id + " says " + data.GetString());
        }
    }
}
