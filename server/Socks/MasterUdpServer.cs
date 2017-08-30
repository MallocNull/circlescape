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

        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;

            Clients = new Dictionary<string, Client>();
            short port = (short)Configuration.General["Master Port"];
            Sock = new UdpClient(new IPEndPoint(IPAddress.Any, port));
            
            IsOpen = true;
            ListeningThread = new Thread(Listener);
            ListeningThread.Start();
        }

        private static bool IsClientConnected(string client) {
            return Clients.ContainsKey(client);
        }

        public static void Listener() {
            while(IsOpen) {
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Any, 0);
                while(Sock.Available > 0) {
                    var data = Sock.Receive(ref endPoint);
                    var client = endPoint.ToString();
                    var encryptor = IsClientConnected(client) ? Clients[client].Encryptor : null;
                    
                    Packet packet = 
                        encryptor == null ? Packet.FromBytes(data) 
                                          : Packet.FromBytes(encryptor.Parse(data));

                    switch((kIntraMasterId)packet.Id) {
                        case kIntraMasterId.InitiationAttempt:
                            if(packet.RegionCount != 1)
                                break;

                            if(packet[0] == Configuration.General["Master Secret"]) {
                                var request = 

                                var request = Key.GenerateRequestPacket().GetBytes();
                                Sock.Send(request, request.Length, endPoint);
                            }
                            break;
                        case kIntraMasterId.KeyExchange:

                            break;
                    }
                }

                Thread.Sleep(1);
            }
        }

        public static void Close() {
            IsOpen = false;
            ListeningThread.Join();
            ListeningThread = null;
            Sock.Dispose();
        }

        class Client {
            public DateTime LastReceive { get; set; }
            public Cipher Encryptor { get; set; }
            public Key Key { get; set; }
        }
    } 
}
