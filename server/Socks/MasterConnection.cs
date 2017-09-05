using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Kneesocks;
using Glove;
using SockScape.Encryption;

namespace SockScape {
    class MasterConnection : Connection {
        private Key Key;
        public Cipher Encryptor { get; private set; }

        
        protected override void OnOpen() {
            Key = new Key();
            Send(Key.GenerateRequestPacket().GetBytes());
        }

        protected override void OnParse() {

        }

        protected override void OnReceive(byte[] data) {
            Packet packet =
                Encryptor == null ? Packet.FromBytes(data)
                                  : Packet.FromBytes(Encryptor.Parse(data));

            if(packet == null) {
                Disconnect(Frame.kClosingReason.ProtocolError, "Packet received was not legal.");
                return;
            }

            switch((kInterMasterId)packet.Id) {
                case kInterMasterId.KeyExchange:
                    Key.ParseResponsePacket(packet);
                    if(!Key.Succeeded) {
                        Disconnect(Frame.kClosingReason.ProtocolError, "Could not exchange keys.");
                        return;
                    }

                    Encryptor = new Cipher(Key.PrivateKey);
                    break;
                case kInterMasterId.LoginAttempt:

                    break;
                case kInterMasterId.RegistrationAttempt:

                    break;
                default:
                    Disconnect(Frame.kClosingReason.ProtocolError, "Packet ID could not be understood at this time.");
                    break;
            }

            Console.WriteLine(Id + " says " + data.GetString());
        }
    }
}
