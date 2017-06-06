using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.DAL {
    public partial class Origin {
        public long Id { get; set; }

        [ForeignKey("User")]
        public long UserId { get; set; }
        public virtual User User { get; set; }

        protected string RawIp { get; set; }
        public IPAddress Ip {
            get {
                return IPAddress.Parse(RawIp);
            }
            set {
                RawIp = value.ToString();
            }
        }
    }
}
