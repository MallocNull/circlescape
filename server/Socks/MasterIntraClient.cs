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
    static class MasterIntraClient {
        private static Key Key;

        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        private static DateTime LastMessageOut = new DateTime(0);
        private static TimeSpan DeltaLastOut
            => DateTime.Now - LastMessageOut;

        private static DateTime LastMessageIn = new DateTime(0);
        private static TimeSpan DeltaLastIn
            => DateTime.Now - LastMessageIn;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;

            ushort port = (ushort)Configuration.General["Master Port"];
            Sock = new UdpClient(Configuration.General["Master Addr"], port);

            Key = new Key();
            Encryptor = null;

            IsOpen = true;
            ListeningThread = new Thread(Listener);
            ListeningThread.Start();
        }

        public static void Listener() {
            while(IsOpen) {
                var endPoint = new IPEndPoint(0, 0);
                while(Sock.Available > 0) {
                    var data = Sock.Receive(ref endPoint);
                    LastMessageIn = DateTime.Now;

                    Packet packet =
                        Encryptor == null ? Packet.FromBytes(data)
                                          : Packet.FromBytes(Encryptor.Parse(data));

                    switch((kIntraMasterId)packet.Id) {
                        case kIntraMasterId.KeyExchange:
                            var responsePacket = Key.ParseRequestPacket(packet);
                            Encryptor = new StreamCipher(Key.PrivateKey);
                            if(responsePacket != null)
                                Send(responsePacket);
                            else
                                LastMessageIn = new DateTime(0);
                            break;

                        case kIntraMasterId.PositiveAck:
                            Console.WriteLine($"Packet type {packet[0]} accepted by master");
                            break;

                        case kIntraMasterId.NegativeAck:
                            Console.WriteLine($"Packet type {packet[0]} declined by master for reason {packet[1]}");
                            break;

                        case kIntraMasterId.EncryptionError:
                            NextSendId = NextRecvId = 0;
                            Buffer.Clear();
                            Key = new Key();
                            Encryptor = null;
                            LastMessageIn = new DateTime(0);
                            break;
                    }
                }

                if(LastMessageIn.Ticks != 0) {
                    if(DeltaLastOut.TotalSeconds > 2)
                        Send(Encryptor.Parse(ServerContext.StatusUpdatePacket.GetBytes()));
                } else
                    if(DeltaLastOut.TotalSeconds > 10)
                        Send(new Packet(kIntraSlaveId.InitiationAttempt, Configuration.General["Master Secret"]));

                Thread.Sleep(1);
            }
        }

        public static void Send(Packet packet) {
            Send(packet.GetBytes());
        }

        public static void Send(byte[] bytes) {
            Sock.Send(bytes, bytes.Length);
            LastMessageOut = DateTime.Now;
            Buffer.Add(NextSendId, bytes);
            ++NextSendId;
        }

        public static void Close() {
            IsOpen = false;
            ListeningThread.Join();
            ListeningThread = null;
        }
    }
}
