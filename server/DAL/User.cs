using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Data.Entity.Spatial;

namespace CircleScape.DAL {
    public partial class User {
        public long Id { get; set; }

        [Required]
        public string Username { get; set; }

        [Required]
        public string Password { get; set; }

        public long? Joined { get; set; }

        public long? LastLogin { get; set; }

        [Required]
        public string JoinedIp { get; set; }
        
        public string LastIp { get; set; }
        
        public virtual ICollection<Session> Sessions { get; set; }
        public virtual ICollection<Item> Items { get; set; }
    }
}
