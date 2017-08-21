using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Glove.INI {
    public class Value {
        private readonly string Raw;

        public Value(string raw) {
            Raw = raw;
        }

        public static implicit operator string(Value value) {
            return value.Raw;
        }

        public static implicit operator bool(Value value) {
            return Boolean.TryParse(value.Raw, out bool retval) && retval;
        }

        public static implicit operator Int32(Value value) {
            return Int32.TryParse(value.Raw, out Int32 retval)
                ? retval
                : 0;
        }

        public static implicit operator double(Value value) {
            return Double.TryParse(value.Raw, out double retval)
                ? retval
                : 0;
        }
    }
}
