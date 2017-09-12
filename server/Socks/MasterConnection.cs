using System;
using System.Collections.Generic;
using System.Data.Entity;
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
            Send(Key.GenerateRequestPacket());
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
                    if(packet.RegionCount != 3 || !packet.CheckRegions(2, 2))
                        break;

                    using(var db = new ScapeDb()) {
                        User user;
                        if((user = db.Users.FirstOrDefault(x => x.Username == packet[0])) == null) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "User does not exist."));
                            break;
                        }

                        if(!packet[1].Str.CheckPassword(user.Password)) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "Password is incorrect."));
                            break;
                        }

                        if(user.Session?.DeltaLastPing.TotalMinutes < 3) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "You are already logged in. Log out or try again shortly."));
                            break;
                        }

                        ushort server = packet[2].Raw.UnpackUInt16();
                        if(!MasterServerList.HasId(server)) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "The world you have specified is offline."));
                            break;
                        }

                        if(user.Session?.DeltaLastPing.TotalMinutes >= 3) {
                            db.Entry(user.Session).State = EntityState.Deleted;
                            db.SaveChanges();
                        }

                        db.Sessions.Add(new Session {
                            Id = user.Id,
                            Secret = RNG.NextBytes(16),
                            ServerId = server,
                            LastPing = DateTime.UtcNow
                        });
                        db.SaveChanges();
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

        private void Send(Packet packet) {
            Send(packet.GetBytes());
        }

        private void SendEncrypted(Packet packet) {
            Send(Encryptor.Parse(packet.GetBytes()));
        }
    }
}
