using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Glove;
using SockScape.DAL;
using SockScape.Encryption;

namespace SockScape {
    static class MasterUdpServer {
        private static Dictionary<string, Client> Prospects;
        private static Dictionary<string, Client> Clients;

        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;
            
            MasterServerList.Clear();
            Prospects = new Dictionary<string, Client>();
            Clients = new Dictionary<string, Client>();

            ushort port = (ushort)Configuration.General["Master Port"];
            Sock = new UdpClient(new IPEndPoint(IPAddress.Any, port));
            
            IsOpen = true;
            ListeningThread = new Thread(Listener);
            ListeningThread.Start();
        }

        public static void Listener() {
            while(IsOpen) {
                IPEndPoint endPoint = new IPEndPoint(0, 0);
                while(Sock.Available > 0) {
                    var data = Sock.Receive(ref endPoint);
                    var client = endPoint.ToString();
                    var encryptor = IsClientConnected(client) ? Clients[client].Encryptor : null;
                    if(data.Take(Packet.MagicNumber.Length).SequenceEqual(Packet.MagicNumber))
                        encryptor = null;
                    
                    Packet packet = 
                        encryptor == null ? Packet.FromBytes(data) 
                                          : Packet.FromBytes(encryptor.Parse(data));

                    if(packet == null) {
                        if(encryptor != null)
                            EncryptionError(endPoint);
                        continue;
                    }

                    Clients[client].LastReceive = DateTime.Now;
                    switch((kIntraSlaveId)packet.Id) {
                        case kIntraSlaveId.InitiationAttempt:
                            if(packet.RegionCount != 1 || IsProspectConnected(client))
                                break;

                            if(packet[0] == Configuration.General["Master Secret"]) {
                                var key = new Key();
                                Prospects[client] = new Client {
                                    LastReceive = DateTime.Now,
                                    Address = endPoint,
                                    Key = key
                                };
                                
                                Send(key.GenerateRequestPacket(), endPoint);
                            }
                            break;

                        case kIntraSlaveId.KeyExchange:
                            if(!IsProspectConnected(client))
                                break;

                            var privateKey = Prospects[client].Key.ParseResponsePacket(packet);
                            if(privateKey != -1) {
                                Prospects[client].LastReceive = DateTime.Now;
                                Prospects[client].Encryptor = new StreamCipher(privateKey);
                                Clients[client] = Prospects[client];
                                Prospects.Remove(client);
                            } else
                                Prospects.Remove(client);
                            break;
                            
                        case kIntraSlaveId.StatusUpdate:
                            if(!IsClientConnected(client) || packet.RegionCount < 1)
                                break;

                            if(packet.CheckRegions(0, 1)) {
                                NegativeAck(endPoint, encryptor, kIntraSlaveId.StatusUpdate, "Server count is malformed.");
                                break;
                            }

                            byte serverCount = packet[0].Raw[0];
                            if(packet.RegionCount != 1 + 3 * serverCount) {
                                NegativeAck(endPoint, encryptor, kIntraSlaveId.StatusUpdate, "Region count does not match server count");
                                break;
                            }

                            for(byte i = 0; i < serverCount; ++i) {
                                if(!packet.CheckRegions(2 + 3 * i, 2, 2, 2))
                                    continue;

                                MasterServerList.Write(new Server {
                                    Id = packet[2 + 3 * i].Raw.UnpackUInt16(),
                                    UserCount = packet[3 + 3 * i].Raw.UnpackUInt16(),
                                    Address = endPoint.Address,
                                    Port = packet[4 + 3 * i].Raw.UnpackUInt16()
                                });
                            }

                            PositiveAck(endPoint, encryptor, kIntraSlaveId.StatusUpdate);
                            break;
                    }
                }



                Thread.Sleep(1);
            }
        }

        private static void Send(Packet packet, IPEndPoint client) {
            Send(packet.GetBytes(), client);
        }

        private static void Send(byte[] bytes, IPEndPoint client) {
            Sock.Send(bytes, bytes.Length, client);
        }

        public static void Close() {
            IsOpen = false;
            ListeningThread.Join();
            ListeningThread = null;
            Sock.Dispose();
        }
        
        private static bool IsProspectConnected(string client) {
            return Prospects.ContainsKey(client);
        }

        private static bool IsClientConnected(string client) {
            return Clients.ContainsKey(client);
        }

        private static void PositiveAck(IPEndPoint endPoint, StreamCipher cipher, kIntraSlaveId id) {
            Send(cipher.Parse(new Packet(kIntraMasterId.PositiveAck, id).GetBytes()), endPoint);
        }

        private static void NegativeAck(IPEndPoint endPoint, StreamCipher cipher, kIntraSlaveId id, string message = "An error occurred while parsing a packet.") {
            Send(cipher.Parse(new Packet(kIntraMasterId.NegativeAck, id, message).GetBytes()), endPoint);
        }

        private static void EncryptionError(IPEndPoint endPoint, string message = "A general encryption error has occurred. Renegotiation required.") {
            Send(new Packet(kIntraMasterId.EncryptionError, message), endPoint);
        }

        class Client {
            public IPEndPoint Address { get; set; }
            public DateTime LastReceive { get; set; }
            public TimeSpan ReceiveDelta => DateTime.Now - LastReceive;
            public StreamCipher Encryptor { get; set; }
            public Key Key { get; set; }
        }
    } 
}
