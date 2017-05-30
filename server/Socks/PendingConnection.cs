using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Square;
using Kneesocks;
using CircleScape.Encryption;

namespace CircleScape {
    class PendingConnection : Connection {
        private DateTime ConnectionOpened;
        private Key Key;
        private Cipher Encryptor;

        protected override void OnOpen() {
            ConnectionOpened = DateTime.UtcNow;
            Key = new Key();

            Send(Key.GenerateRequestPacket().GetBytes());
        }

        protected override void OnParse() {
            if((DateTime.UtcNow - ConnectionOpened).Seconds > 60) {
                Disconnect(Frame.kClosingReason.ProtocolError, "Logon request timed out.");
            }
        }

        protected override void OnReceive(byte[] data) {
            var packet = Packet.FromBytes(data);
            if(!packet.IsLegal) {
                Disconnect(Frame.kClosingReason.ProtocolError, "Packet received was not legal.");
                return;
            }

            switch(packet.Id) {
                case Packet.kId.KeyExchange:
                    Key.ParseResponsePacket(packet);
                    if(!Key.Succeeded) {
                        Disconnect(Frame.kClosingReason.ProtocolError, "Could not exchange keys.");
                        return;
                    }

                    Encryptor = new Cipher(Key.PrivateKey);
                    break;
            }

            Console.WriteLine(Id + " says " + data.GetString());
        }
    }
}
