namespace CircleScape.DAL
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;
    using System.ComponentModel.DataAnnotations.Schema;
    using System.Data.Entity.Spatial;

    public partial class User
    {
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2214:DoNotCallOverridableMethodsInConstructors")]
        public User()
        {
            Sessions = new HashSet<Session>();
        }

        public long Id { get; set; }

        [Required]
        [StringLength(2147483647)]
        public string Username { get; set; }

        [Required]
        [StringLength(2147483647)]
        public string Password { get; set; }

        public long? Joined { get; set; }

        public long? LastLogin { get; set; }

        [Required]
        [StringLength(2147483647)]
        public string JoinedIp { get; set; }

        [StringLength(2147483647)]
        public string LastIp { get; set; }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2227:CollectionPropertiesShouldBeReadOnly")]
        public virtual ICollection<Session> Sessions { get; set; }
    }
}
