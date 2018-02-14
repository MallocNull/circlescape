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
    static class MasterIntraServer {
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

            // TODO figure out what this has to do with ICMP (in client too)
            /*uint IOC_IN            = 0x80000000,
                 IOC_VENDOR        = 0x18000000,
                 SIO_UDP_CONNRESET = IOC_IN | IOC_VENDOR | 12;
            Sock.Client.IOControl((int)SIO_UDP_CONNRESET, new byte[] {0}, null);*/

            IsOpen = true;
            ListeningThread = new Thread(Listener);
            ListeningThread.Start();
        }

        public static void Listener() {
            while(IsOpen) {
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, 0);
                while(Sock.Available > 0) {
                    var data = Sock.Receive(ref endPoint);
                    var client = endPoint.ToString();
                    var encryptor = IsClientConnected(client) ? Clients[client].Encryptor : null;
                    if(data.Take(Packet.MagicNumber.Length).SequenceEqual(Packet.MagicNumber))
                        encryptor = null;
                    
                    Packet packet = 
                        encryptor == null ? Packet.FromBytes(data) 
                                          : Packet.FromBytes(encryptor.Decrypt(data));

                    if(packet == null) {
                        if(encryptor != null)
                            EncryptionError(endPoint);
                        continue;
                    }

                    if(IsProspectConnected(client) && encryptor == null)
                        Prospects[client].LastReceive = DateTime.UtcNow;
                    else if(IsClientConnected(client) && encryptor != null)
                        Clients[client].LastReceive = DateTime.UtcNow;

                    switch((kIntraSlaveId)packet.Id) {
                        case kIntraSlaveId.InitiationAttempt:
                            if(packet.RegionCount != 1 || IsProspectConnected(client))
                                break;

                            if(packet[0] == Configuration.General["Master Secret"]) {
                                var key = new Key();
                                Prospects[client] = new Client {
                                    LastReceive = DateTime.UtcNow,
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
                                Prospects[client].LastReceive = DateTime.UtcNow;
                                Prospects[client].Encryptor = new BlockCipher(privateKey);
                                Clients[client] = Prospects[client];
                                Prospects.Remove(client);
                            } else
                                Prospects.Remove(client);
                            break;
                            
                        case kIntraSlaveId.StatusUpdate:
                            if(!IsClientConnected(client) || packet.RegionCount < 1)
                                break;

                            if(!packet.CheckRegionLengths(0, 1)) {
                                NegativeAck(endPoint, encryptor, kIntraSlaveId.StatusUpdate, "Server count is malformed.");
                                break;
                            }

                            byte serverCount = packet[0].Raw[0];
                            if(packet.RegionCount != 1 + 3 * serverCount) {
                                NegativeAck(endPoint, encryptor, kIntraSlaveId.StatusUpdate, "Region count does not match server count");
                                break;
                            }

                            for(byte i = 0; i < serverCount; ++i) {
                                if(!packet.CheckRegionLengths(1 + 3 * i, 2, 2, 2))
                                    continue;

                                MasterServerList.Write(new Server {
                                    Id = packet[1 + 3 * i].Raw.UnpackUInt16(),
                                    UserCount = packet[2 + 3 * i].Raw.UnpackUInt16(),
                                    Address = endPoint.Address,
                                    Port = packet[3 + 3 * i].Raw.UnpackUInt16(),
                                    Owner = Clients[client]
                                });
                            }

                            PositiveAck(endPoint, encryptor, kIntraSlaveId.StatusUpdate);
                            break;
                    }
                }

                Prospects = Prospects.Where(x => x.Value.ReceiveDelta.TotalSeconds < 10)
                                     .ToDictionary(x => x.Key, x => x.Value);

                var expiredClients = Clients.Where(x => x.Value.ReceiveDelta.TotalSeconds > 60).Select(x => x.Value).ToList();
                if(expiredClients.Count > 0)
                    MasterServerList.RemoveServersByOwners(expiredClients);

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

        private static void PositiveAck(IPEndPoint endPoint, BlockCipher cipher, kIntraSlaveId id) {
            Send(cipher.Encrypt(new Packet(kIntraMasterId.PositiveAck, (byte)id).GetBytes()), endPoint);
        }

        private static void NegativeAck(IPEndPoint endPoint, BlockCipher cipher, kIntraSlaveId id, string message = "An error occurred while parsing a packet.") {
            Send(cipher.Encrypt(new Packet(kIntraMasterId.NegativeAck, (byte)id, message).GetBytes()), endPoint);
        }

        private static void EncryptionError(IPEndPoint endPoint, string message = "A general encryption error has occurred. Renegotiation required.") {
            Send(new Packet(kIntraMasterId.EncryptionError, message), endPoint);
        }

        public class Client {
            public IPEndPoint Address { get; set; }
            public DateTime LastReceive { get; set; }
            public TimeSpan ReceiveDelta => DateTime.UtcNow - LastReceive;
            public BlockCipher Encryptor { get; set; }
            public Key Key { get; set; }
        }
    } 
}
