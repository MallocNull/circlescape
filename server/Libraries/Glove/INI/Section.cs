using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Glove.INI {
    public class Section : IEnumerable<Instance> {
        private readonly List<Instance> Instances = new List<Instance>();

        internal Section() { }

        internal Instance Push() {
            Instances.Add(new Instance());
            return Instances[Instances.Count - 1];
        }

        public string this[string key]
            => Instances[0][key];

        public Instance this[int i]
            => Instances[i];

        public int Count => Instances.Count;

        IEnumerator IEnumerable.GetEnumerator() {
            return Instances.GetEnumerator();
        }

        IEnumerator<Instance> IEnumerable<Instance>.GetEnumerator() {
            return Instances.GetEnumerator();
        }
    }
}
