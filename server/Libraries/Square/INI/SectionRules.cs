using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Square.INI {
    public class SectionRules {
        public string Name { get; set; }
        public bool Required { get; set; } = true;
        public bool AllowMultiple { get; set; } = false;
        public string[] RequiredFields { get; set; } = new string[0];
    }
}
