using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SockScape.DAL {
    class Item {
        public long Id { get; set; }

        [ForeignKey("Master"), Required]
        public int MasterId { get; set; }
        public virtual ItemMaster Master { get; set; }

        [ForeignKey("User"), Required]
        public long UserId { get; set; }
        public virtual User User { get; set; }
        
        [Required]
        public long Quantity { get; set; }

        [Required]
        public long Sequence { get; set; }
    }
}
