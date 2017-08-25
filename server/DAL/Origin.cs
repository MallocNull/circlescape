using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace SockScape.DAL {
    public partial class Origin {
        public long Id { get; set; }

        [ForeignKey("User"), Index("IX_RawIp_UserId_Unique", 1, IsUnique = true)]
        [Required]
        public long UserId { get; set; }
        public virtual User User { get; set; }

        [Required, Index("IX_RawIp_UserId_Unique", 2, IsUnique = true)]
        public byte[] RawIp { get; set; }

        [NotMapped]
        public IPAddress Ip {
            get => new IPAddress(RawIp);
            set => RawIp = value.MapToIPv6().GetAddressBytes();
        }
    }
}
