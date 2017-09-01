using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Glove;
using SockScape.Encryption;

namespace SockScape {
    static class MasterUdpServer {
        private static Dictionary<string, Client> Prospects;
        private static Dictionary<string, Client> Clients;
        private static ServerList Servers;

        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;

            Servers = new ServerList();
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

                    if(packet == null)
                        break;
                    
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
                                Prospects[client].Encryptor = new Cipher(privateKey);
                                Clients[client] = Prospects[client];
                                Prospects.Remove(client);
                            } else
                                Prospects.Remove(client);
                            break;
                            
                        case kIntraSlaveId.StatusUpdate:
                            if(!IsClientConnected(client) || packet.RegionCount < 1)
                                break;

                            
                            break;
                    }
                }



                Thread.Sleep(1);
            }
        }

        private static void Send(Packet packet, IPEndPoint client) {
            var message = packet.GetBytes();
            Sock.Send(message, message.Length, client);
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

        private static Packet PositiveAck(byte id) {
            return new Packet(kIntraMasterId.PositiveAck, id);
        }

        private static Packet NegativeAck(byte id, string message = "") {
            return new Packet(kIntraMasterId.NegativeAck, id, message);
        }

        private static Packet EncryptionError(string message = "A general encryption error has occurred.") {
            return new Packet(kIntraMasterId.EncryptionError, message);
        }

        class Client {
            public IPEndPoint Address { get; set; }
            public DateTime LastReceive { get; set; }
            public Cipher Encryptor { get; set; }
            public Key Key { get; set; }
        }
    } 
}
