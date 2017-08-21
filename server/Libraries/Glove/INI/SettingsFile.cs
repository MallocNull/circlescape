using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Glove.INI {
    public class SettingsFile {
        private readonly Dictionary<string, Section> Sections 
            = new Dictionary<string, Section>(StringComparer.OrdinalIgnoreCase);

        public SettingsFile(string path) {
            var lines = File.ReadAllLines(path);

            Instance currentInstance = null;
            foreach(var rawLine in lines) {
                var line = rawLine.Trim();
                if(line.StartsWith("!", "#", ";"))
                    continue;

                if(line.StartsWith("[") && line.EndsWith("]")) {
                    var section = line.Substring(1, line.Length - 2);
                    if(!Sections.ContainsKey(section))
                        Sections.Add(section, new Section());

                    currentInstance = Sections[section].Push();
                } else {
                    if(currentInstance != null)
                        currentInstance.Push(line);
                    else
                        throw new FormatException("Non-section line before any define sections in '"+ path +"'");
                }
            }
        }

        public SettingsFile(string path, List<SectionRules> rules) : this(path) {
            foreach(var rule in rules) {
                var name = rule.Name;

                if(ContainsSection(name)) {
                    var section = Sections[name];
                    if(!rule.AllowMultiple && section.Count > 1)
                        throw new FormatException("Section '"+ name +"' is not allowed to have multiple declarations in '"+ path +"'");

                    if(rule.RequiredFields.Length > 0) {
                        foreach(var instance in section) {
                            foreach(var field in rule.RequiredFields) {
                                if(instance.ContainsKey(field))
                                    throw new FormatException("Expected field '"+ field +"' in section '" + name + "' was not found in '" + path + "'");
                            }
                        }
                    }
                } else if(rule.Required)
                    throw new FormatException("Expected section '"+ name +"' was not found in '"+ path +"'");
            }
        }

        public Section this[string section] {
            get {
                if(Sections.ContainsKey(section))
                    return Sections[section];
                else return null;
            }
        }

        public bool ContainsSection(string section) {
            return Sections.ContainsKey(section);
        }
    }
}
