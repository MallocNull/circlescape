using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace SockScape.DAL {
    public partial class Session {
        [Key, ForeignKey("User")]
        public long Id { get; set; }
        [Required]
        public virtual User User { get; set; }

        [Required, Index(IsUnique = true)]
        public byte[] Secret { get; set; }

        [Required]
        public int ServerId { get; set; }
    }
}