using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Kneesocks;
using Glove;
using SockScape.DAL;
using SockScape.Encryption;

namespace SockScape {
    class MasterConnection : Connection {
        private Key Key;
        public StreamCipher Encryptor { get; private set; }
        
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

            if(packet.Id != (int)kInterMasterId.KeyExchange && Encryptor == null) {
                Disconnect(Frame.kClosingReason.ProtocolError, "You must exchange keys before performing any other operations.");
                return;
            }

            switch((kInterMasterId)packet.Id) {
                case kInterMasterId.KeyExchange:
                    Key.ParseResponsePacket(packet);
                    if(!Key.Succeeded) {
                        Disconnect(Frame.kClosingReason.ProtocolError, "Could not exchange keys.");
                        return;
                    }

                    Encryptor = new StreamCipher(Key.PrivateKey);
                    break;
                case kInterMasterId.LoginAttempt:
                    if(packet.RegionCount != 2)
                        break;

                    using(var db = new ScapeDb()) {
                        if(db.Users.)
                    }
                    break;
                case kInterMasterId.RegistrationAttempt:
                    using(var db = new ScapeDb()) {
                        
                    }
                    break;
                default:
                    Disconnect(Frame.kClosingReason.ProtocolError, "Packet ID could not be understood at this time.");
                    break;
            }

            Console.WriteLine($"{Id} says {data.GetString()}");
        }
    }
}
