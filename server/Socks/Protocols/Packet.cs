using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Glove;

namespace SockScape {
    class Packet {
        // (squid)
        public static readonly byte[] MagicNumber = { 0xF0, 0x9F, 0xA6, 0x91 };

        public static Packet FromBytes(byte[] raw) {
            if(raw.Length < 7)
                return null;

            Packet packet = new Packet();

            if(!raw.Subset(0, 4).SequenceEqual(MagicNumber))
                return null;

            packet.Id = raw[4];
            var regionCount = raw[5];
            var regionLengths = new List<uint>();
            var headerPtr = 6;
            for(var i = 0; i < regionCount; ++i) {
                regionLengths.Add(0);
                var first = raw[headerPtr];
                if(first < 254) {
                    regionLengths[i] = first;
                    ++headerPtr;
                } else if(first == 254) {
                    if(headerPtr + 3 < raw.Length)
                        return null;
                    regionLengths[i] = raw.Subset(headerPtr + 1, 2).UnpackUInt16();
                    headerPtr += 3;
                } else {
                    if(headerPtr + 5 < raw.Length)
                        return null;
                    regionLengths[i] = raw.Subset(headerPtr + 1, 4).UnpackUInt32();
                    headerPtr += 5;
                }

                if(headerPtr > raw.Length)
                    return null;
            }

            if(headerPtr + regionLengths.Sum(x => x) > raw.Length)
                return null;
             
            long bodyPtr = headerPtr;
            foreach(var regionLength in regionLengths) {
                // FLAG this could fail if one region exceeds 2^31-1 in size, check later
                packet.Regions.Add(raw.Subset((int)bodyPtr, (int)regionLength));
                bodyPtr += regionLength;
            }

            return packet;
        }
        
        private readonly List<byte[]> Regions = new List<byte[]>();
        public int Id { get; private set; }
        public int RegionCount
            => Regions.Count;

        protected Packet() { }

        public Packet(Enum id, params object[] regions) {
            Initialize((int)Convert.ChangeType(id, id.GetTypeCode()), regions);
        }

        public Packet(int id, params object[] regions) {
            Initialize(id, regions);
        }

        private void Initialize(int id, object[] regions) {
            Id = id;

            foreach(var region in regions)
                AddRegion(region);
        }

        public Region this[int i] 
            => new Region(Regions[i]);

        public Packet AddRegion(object region) {
            if(region.GetType() == typeof(byte[]))
                Regions.Add((byte[])region);
            else if(region.GetType() == typeof(string))
                Regions.Add(((string)region).GetBytes());

            return this;
        }

        public byte[] GetBytes() {
            var header = new List<byte>();
            header.AddRange(MagicNumber);
            header.Add((byte)Id);
            header.Add((byte)RegionCount);
            
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
            public byte[] Data { get; }

            public Region(byte[] data) {
                Data = data;
            }

            public static implicit operator byte[] (Region region) => region.Data;
            public string Bytes
                => this;

            public static implicit operator string(Region region) {
                try {
                    return Encoding.UTF8.GetString(region.Data);
                } catch {
                    return Encoding.ASCII.GetString(region.Data);
                }
            }
            public string Str
                => this;
        }
    }
}
