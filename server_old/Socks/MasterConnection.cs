using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Kneesocks;
using Glove;
using SockScape.DAL;
using SockScape.Encryption;

namespace SockScape {
    class MasterConnection : Connection {
        private readonly Regex UsernameRegex = new Regex("[A-Z0-9_]", RegexOptions.IgnoreCase);
        private readonly Regex EmailRegex = new Regex(@"\B[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}\B", RegexOptions.IgnoreCase);

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

            // TODO rate limiting by ip
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
                    if(packet.RegionCount != 3 || !packet.CheckRegionsMaxLength(0, 16, 255) || !packet.CheckRegionLengths(2, 2))
                        break;

                    Session session;
                    using(var db = new ScapeDb()) {
                        User user;
                        if((user = db.Users.FirstOrDefault(x => x.Username == packet[0])) == null) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "User does not exist."));
                            break;
                        }

                        if(packet[1].Str.Trim() == "" || !packet[1].Str.CheckPassword(user.Password)) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "Password is incorrect."));
                            break;
                        }

                        if(user.Session?.DeltaLastPing.TotalMinutes < 3) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "You are already logged in. Log out or try again shortly."));
                            break;
                        }

                        ushort serverId = packet[2].Raw.UnpackUInt16();
                        if(!MasterServerList.HasId(serverId)) {
                            SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(false), "The world you have specified is offline."));
                            break;
                        }

                        if(user.Session?.DeltaLastPing.TotalMinutes >= 3) {
                            db.Entry(user.Session).State = EntityState.Deleted;
                            db.SaveChanges();
                        }

                        db.Sessions.Add(session = new Session {
                            Id = user.Id,
                            Secret = RNG.NextBytes(16),
                            ServerId = serverId,
                            LastPing = DateTime.UtcNow
                        });

                        db.SaveChanges();
                    }

                    var server = MasterServerList.Get((ushort)session.ServerId);
                    SendEncrypted(new Packet(kInterMasterId.LoginAttempt, Convert.ToByte(true), session.Secret, server.Address.ToString(), server.Port.Pack()));
                    break;
                case kInterMasterId.RegistrationAttempt:
                    if(packet.RegionCount != 3 || !packet.CheckAllMaxLength(0xFF))
                        break;

                    if(!packet[0].Raw.IsAsciiString()) {
                        SendEncrypted(new Packet(kInterMasterId.RegistrationAttempt, Convert.ToByte(false), "Your username cannot contain unicode characters."));
                        break;
                    }

                    string username = packet[0].Str.Trim(),
                           password = packet[1].Str.Trim(),
                           email    = packet[2].Str.Trim();

                    if(username.Length > 16 || !UsernameRegex.IsMatch(username)) {
                        SendEncrypted(new Packet(kInterMasterId.RegistrationAttempt, Convert.ToByte(false), "The username is max 16 characters and can only be letters, numbers, and underscores."));
                        break;
                    }

                    if(!EmailRegex.IsMatch(email)) {
                        SendEncrypted(new Packet(kInterMasterId.RegistrationAttempt, Convert.ToByte(false), "The email address is malformed."));
                        break;
                    }

                    using(var db = new ScapeDb()) {
                        if(db.Users.FirstOrDefault(x => x.Username == username) != null) {
                            SendEncrypted(new Packet(kInterMasterId.RegistrationAttempt, Convert.ToByte(false), "This username is already in use."));
                            break;
                        }

                        if(db.Users.FirstOrDefault(x => x.Email == email) != null) {
                            SendEncrypted(new Packet(kInterMasterId.RegistrationAttempt, Convert.ToByte(false), "This email address is already in use."));
                            break;
                        }

                        // TODO email activation
                        db.Users.Add(new User {
                            Username = username,
                            Password = password.HashPassword(),
                            Email = email,
                            Joined = DateTime.UtcNow
                        });

                        db.SaveChanges();
                    }

                    SendEncrypted(new Packet(kInterMasterId.RegistrationAttempt, Convert.ToByte(true), "Registration was successful."));
                    break;
                case kInterMasterId.ServerListing:
                    SendEncrypted(MasterServerList.ReportPacket);
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
