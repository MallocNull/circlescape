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
        private static BlockCipher Encryptor;

        private static UdpClient Sock;
        private static Thread ListeningThread;
        private static bool IsOpen;

        private static DateTime LastMessageOut = new DateTime(0);
        private static TimeSpan DeltaLastOut
            => DateTime.UtcNow - LastMessageOut;

        private static DateTime LastMessageIn = new DateTime(0);
        private static TimeSpan DeltaLastIn
            => DateTime.UtcNow - LastMessageIn;

        public static void Initialize() {
            if(IsOpen || ListeningThread != null)
                return;

            ushort port = (ushort)Configuration.General["Master Port"];
            Sock = new UdpClient(Configuration.General["Master Addr"], port);

            // TODO figure out what this has to do with ICMP (in server too)
            uint IOC_IN            = 0x80000000,
                 IOC_VENDOR        = 0x18000000,
                 SIO_UDP_CONNRESET = IOC_IN | IOC_VENDOR | 12;
            Sock.Client.IOControl((int)SIO_UDP_CONNRESET, new byte[] {0}, null);

            Key = new Key();
            Encryptor = null;

            IsOpen = true;
            ListeningThread = new Thread(Listener);
            ListeningThread.Start();
        }

        public static void Listener() {
            while(IsOpen) {
                var endPoint = new IPEndPoint(IPAddress.Any, 0);
                while(Sock.Available > 0) {
                    var data = Sock.Receive(ref endPoint);
                    LastMessageIn = DateTime.UtcNow;

                    bool readRaw = Encryptor == null 
                        || data.Take(Packet.MagicNumber.Length).SequenceEqual(Packet.MagicNumber);

                    Packet packet =
                        readRaw ? Packet.FromBytes(data)
                                : Packet.FromBytes(Encryptor.Decrypt(data));

                    switch((kIntraMasterId)packet.Id) {
                        case kIntraMasterId.KeyExchange:
                            var responsePacket = Key.ParseRequestPacket(packet);
                            Encryptor = new BlockCipher(Key.PrivateKey);
                            if(responsePacket != null)
                                Send(responsePacket);
                            else
                                LastMessageIn = new DateTime(0);
                            break;

                        case kIntraMasterId.PositiveAck:
                            Console.WriteLine($"Packet type {packet[0].Raw[0]} accepted by master");
                            break;

                        case kIntraMasterId.NegativeAck:
                            Console.WriteLine($"Packet type {packet[0].Raw[0]} declined by master for reason {packet[1].Str}");
                            break;

                        case kIntraMasterId.EncryptionError:
                            Key = new Key();
                            Encryptor = null;
                            LastMessageIn = new DateTime(0);
                            break;
                    }
                }

                if(Encryptor != null) {
                    if(DeltaLastOut.TotalSeconds > 2)
                        SendEncrypted(ServerContext.StatusUpdatePacket);
                } else
                    if(DeltaLastOut.TotalSeconds > 10)
                        Send(new Packet(kIntraSlaveId.InitiationAttempt, Configuration.General["Master Secret"].Str));

                Thread.Sleep(1);
            }
        }

        public static void Send(Packet packet) {
            Send(packet.GetBytes());
        }

        public static void SendEncrypted(Packet packet) {
            Send(Encryptor.Encrypt(packet.GetBytes()));
        }

        public static void Send(byte[] bytes) {
            Sock.Send(bytes, bytes.Length);
            LastMessageOut = DateTime.UtcNow;
        }

        public static void Close() {
            IsOpen = false;
            ListeningThread.Join();
            ListeningThread = null;
        }
    }
}
