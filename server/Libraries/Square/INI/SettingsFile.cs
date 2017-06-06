using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Square.INI {
    public class SettingsFile {
        private Dictionary<string, Section> Sections 
            = new Dictionary<string, Section>(StringComparer.OrdinalIgnoreCase);

        public SettingsFile(string path) {
            var lines = File.ReadAllLines(path);

            string currentSection = null;
            foreach(var line in lines) {

            }
        }

        public SettingsFile(string path, List<SectionRules> rules) {

        }

        public Section this[string section] {
            get {
                if(Sections.ContainsKey(section))
                    return Sections[section];
                else return null;
            }
        }
    }
}
