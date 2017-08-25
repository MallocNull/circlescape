using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SockScape.DAL {
    public partial class User {
        public long Id { get; set; }

        [Required, MaxLength(16)]
        public string Username { get; set; }

        [Required]
        public byte[] Password { get; set; }

        [Required]
        public DateTime Joined { get; set; }
        public DateTime? LastLogin { get; set; }

        public virtual ICollection<Origin> Origins { get; set; }
        public virtual Session Session { get; set; }
    }
}