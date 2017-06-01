namespace CircleScape.DAL
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;
    using System.ComponentModel.DataAnnotations.Schema;
    using System.Data.Entity.Spatial;

    public partial class Session
    {
        public long Id { get; set; }

        public long UserId { get; set; }

        [Required]
        [StringLength(2147483647)]
        public string IpAddress { get; set; }

        public virtual User User { get; set; }
    }
}
