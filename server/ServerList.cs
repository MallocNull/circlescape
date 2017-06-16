using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape {
    static class ServerList {
        public static Dictionary<int, IPEndPoint> Servers { get; private set; } 
            = new Dictionary<int, IPEndPoint>();

        
    }
}
