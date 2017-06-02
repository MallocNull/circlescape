using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.DAL {
    public class Item {
        public long Id { get; set; }

        [Index("ItemItemDefinitionId")]
        public long DefinitionId { get; set; }
        public virtual ItemDefinition Definition { get; set; }

        [Index("ItemUserId")]
        public long UserId { get; set; }
        public virtual User User { get; set; }

        public long Quantity { get; set; }
    }
}
