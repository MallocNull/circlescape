using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Data.Entity.Spatial;

namespace CircleScape.DAL {
    public partial class Session {
        public long Id { get; set; }

        [Index("SessionUserId")]
        public long UserId { get; set; }
        public virtual User User { get; set; }

        [Required]
        public string IpAddress { get; set; }
    }
}
