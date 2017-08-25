using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SockScape.DAL {
    class ItemMaster {
        public int Id { get; set; }

        [Required, MaxLength(64)]
        public string Name { get; set; }
        [Required, MaxLength(256)]
        public string Description { get; set; }
        
        [Required, DefaultValue(false)]
        public bool Stackable { get; set; }
        [Required, DefaultValue(UInt32.MaxValue)]
        public long MaxStack { get; set; }
        [Required, DefaultValue(true)]
        public bool Droppable { get; set; }

        [Required, MaxLength(64)]
        public string Sheet { get; set; }
        [Required]
        public int Row { get; set; }
        [Required]
        public int Column { get; set; }

        [MaxLength(64)]
        public string Object { get; set; }
    }
}
