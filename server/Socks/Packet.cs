using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Square;

namespace CircleScape {
    class Packet {
        public enum kId {
            KeyExchange = 0,
            LoginAttempt,
            RegistrationAttempt
        }
        
        private static Packet ErrorPacket {
            get => new Packet { IsLegal = false };
        }

        public static Packet FromBytes(byte[] raw) {
            if(raw.Length < 3)
                return ErrorPacket;

            Packet packet = new Packet();
            if(!Enum.IsDefined(typeof(kId), (int)raw[0]))
                return ErrorPacket;
            packet.Id = (kId)raw[0];
            var regionCount = raw[1];
            var regionLengths = new List<uint>();
            var headerPtr = 2;
            for(var i = 0; i < regionCount; ++i) {
                regionLengths.Add(0);
                var first = raw[headerPtr];
                if(first < 254) {
                    regionLengths[i] = first;
                    ++headerPtr;
                } else if(first == 254) {
                    if(headerPtr + 3 < raw.Length)
                        return ErrorPacket;
                    regionLengths[i] = raw.Subset(headerPtr + 1, 2).UnpackUInt16();
                    headerPtr += 3;
                } else {
                    if(headerPtr + 5 < raw.Length)
                        return ErrorPacket;
                    regionLengths[i] = raw.Subset(headerPtr + 1, 4).UnpackUInt32();
                    headerPtr += 5;
                }

                if(headerPtr > raw.Length)
                    return ErrorPacket;
            }

            if(headerPtr + regionLengths.Sum(x => x) > raw.Length)
                return ErrorPacket;
             
            long bodyPtr = headerPtr;
            foreach(var regionLength in regionLengths) {
                // FLAG this could fail if one region exceeds 2^31-1 in size, check later
                packet.Regions.Add(raw.Subset((int)bodyPtr, (int)regionLength));
                bodyPtr += regionLength;
            }

            return packet;
        }
        
        private List<byte[]> Regions = new List<byte[]>();
        public kId Id { get; private set; } = kId.KeyExchange;
        public bool IsLegal { get; private set; } = true;
        public int RegionCount {
            get => Regions.Count;
        }

        private Packet() { }

        public Packet(kId id, params object[] regions) {
            Id = id;

            foreach(var region in regions)
                AddRegion(region);
        }

        public Region this[int i] {
            get => new Region(Regions[i]);
        }

        public Packet AddRegion(object region) {
            if(region.GetType() == typeof(byte[]))
                Regions.Add((byte[])region);
            else if(region.GetType() == typeof(string))
                Regions.Add(((string)region).GetBytes());

            return this;
        }

        public byte[] GetBytes() {
            if(!IsLegal)
                return null;

            var header = new List<byte> {
                (byte)Id,
                (byte)RegionCount
            };
            IEnumerable<byte> body = new byte[0];
            foreach(var region in Regions) {
                if(region.Length < 0xFE)
                    header.Add((byte)region.Length);
                else if(region.Length <= 0xFFFF) {
                    header.Add(0xFE);
                    header.AddRange(((UInt16)region.Length).Pack());
                } else {
                    header.Add(0xFF);
                    header.AddRange(region.Length.Pack());
                }

                body = body.Concat(region);
            }

            return header.Concat(body).ToArray();
        }

        public class Region {
            public byte[] Data { get; private set; }

            public Region(byte[] data) {
                Data = data;
            }

            public static implicit operator byte[] (Region region) => region.Data;
            public string Bytes {
                get => this;
            }

            public static implicit operator string(Region region) {
                try {
                    return Encoding.UTF8.GetString(region.Data);
                } catch {
                    return Encoding.ASCII.GetString(region.Data);
                }
            }
            public string Str {
                get => this;
            }
        }
    }
}
