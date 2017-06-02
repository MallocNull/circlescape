using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Data.Entity.Spatial;

namespace CircleScape.DAL {
    public partial class ItemDefinition {
        public long Id { get; set; }

        [Required]
        public string Name { get; set; }

        [Required]
        public string Sheet { get; set; }
        public long Offset { get; set; }
        
        public bool Stackable { get; set; } = false;

        public virtual ICollection<Item> Items { get; set; }
    }
}
