using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Glove.INI;

namespace SockScape {
    public static class Configuration {
        private static readonly SettingsFile Settings;

        static Configuration() {
            Settings = new SettingsFile(
                "config.ini",
                new List<SectionRules> {
                    new SectionRules {
                        Name = "General",
                        Required = true,
                        RequiredFields = new[] {
                            "Run Master",
                            "Master Port",
                            "Master Addr",
                            "Master Secret",
                            "Master IV",
                            "Max Users"
                        }
                    },

                    new SectionRules {
                        Name = "Mail",
                        Required = true,
                        RequiredFields = new[] {
                            "Host",
                            "UseTLS",
                            "Auth"
                        }
                    },

                    new SectionRules {
                        Name = "Database",
                        Required = true,
                        RequiredFields = new[] {
                            "Server",
                            "Username",
                            "Password",
                            "Database"
                        }
                    },

                    new SectionRules {
                        Name = "Server",
                        AllowMultiple = true,
                        Required = true,
                        RequiredFields = new[] {
                            "Id",
                            "Port"
                        }
                    }
                }
            );
        }

        public static Section Get(string section) {
            return Settings[section];
        }

        public static Instance General 
            => Settings["General"][0];

        public static Instance Database
            => Settings["Database"][0];

        public static Section Servers 
            => Settings["Server"];
    }
}
