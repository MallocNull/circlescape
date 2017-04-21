using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace CircleScape.Websocket {
    class Pool {
        private const int InitialCount = 3;
        private const int InitialSize  = 3;
        private const int SizeGrowth   = 1;
        private const int MaxSize      = 10;

        private int _fullThreadCount;
        private bool updateFullThreadCount = true;

        private List<ThreadContext> Threads
            = new List<ThreadContext>();
        private Dictionary<UInt64, Connection> Connections
            = new Dictionary<UInt64, Connection>();

        public Pool() {
            for(var i = 0; i < InitialCount; ++i) {
                
            }
        }

        public bool AddConnection(Connection connection) {
            foreach(var thread in Threads) {

            }
        }

        private ThreadContext CreateThread(Connection initialConnection = null) {
            var stack = new Stack(true);
            var ctx = new ThreadContext {
                Stack = stack,
                Thread = new Thread(new ThreadStart(stack.ManageStack))
            };

            Threads.Add(ctx);
            return ctx;
        }

        private int FullThreadCount {
            get {
                if(updateFullThreadCount) {
                    _fullThreadCount = Math.Min(
                            MaxSize, 
                            InitialSize + SizeGrowth * (Threads.Count - InitialCount)
                        );
                }

                return _fullThreadCount;
            }
        }

        class ThreadContext {
            public Thread Thread { get; set; }
            public Stack Stack { get; set; }
        }
    }
}
