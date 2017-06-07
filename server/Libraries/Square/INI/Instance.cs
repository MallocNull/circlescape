using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Square.INI {
    public class Instance : IEnumerable<KeyValuePair<string, Value>> {
        private Dictionary<string, Value> Data 
            = new Dictionary<string, Value>(StringComparer.OrdinalIgnoreCase);

        internal Instance() { }

        internal void Push(string line) {
            if(line.Contains('=')) {
                var parts = line.Split(new char[] { '=' }, 2);
                Data.Add(parts[0].Trim(), new Value(parts[1].Trim()));
            } else
                throw new FormatException("Line is not a key-value pair delimited by an equals sign.");
        }

        public Value this[string key] {
            get {
                if(Data.ContainsKey(key))
                    return Data[key];
                else return null;
            }
        }

        public bool ContainsKey(string key) {
            return Data.ContainsKey(key);
        }

        IEnumerator IEnumerable.GetEnumerator() {
            return Data.GetEnumerator();
        }

        IEnumerator<KeyValuePair<string, Value>> IEnumerable<KeyValuePair<string, Value>>.GetEnumerator() {
            return Data.GetEnumerator();
        }
    }
}
