using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Kneesocks {
    public class Pool<T> where T : Connection {
        // number of threads that should be started when the pool is created
        // these threads will run for as long as the pool exists
        public int InitialCount { get; set; } = 3;
        // amount of connections that should initially be allowed per thread
        public int InitialSize { get; set; } = 3;
        // amount of additional connections that each thread can handle after
        // a new thread is created
        public int SizeGrowth { get; set; } = 1;
        // maximum amount of connections that a single thread will be assigned
        // 0 means no limit
        public int MaxSize { get; set; } = 10;
        // maximum number of threads that will be spawned
        // 0 means no limit
        public int MaxCount { get; set; } = 0;
        // maximum number of connections in a thread that exceeds the calculated
        // amount for the pool's thread count before the connection redistribution
        // function is called
        // 0 means never redistribute
        public int Tolerance { get; set; } = 0;

        private int _fullThreadCount;
        private volatile bool updateFullThreadCount = true;

        private List<ThreadContext> Threads
            = new List<ThreadContext>();

        private long InternalCounter = 0;
        private Dictionary<UInt64, Connection> Connections
            = new Dictionary<UInt64, Connection>();

        public Pool() {
            for(var i = 0; i < InitialCount; ++i)
                CreateThread(runWithNoClients: true);
        }

        private void IndexConnection(UInt64 id, Connection connection) {
            lock(Connections) {
                if(id == 0)
                    id = (ulong)Interlocked.Increment(ref InternalCounter);
                
                connection.Id = id;
                Connections.Add(id, connection);
            }
        }

        public void InvalidateConnection(UInt64 id) {
            lock(Connections) {
                Connections.Remove(id);
            }
        }

        public bool AddConnection(Connection connection, UInt64 id = 0) {
            lock(Threads) {
                foreach(var thread in Threads) {
                    if(thread.Stack.Count < FullThreadSize) {
                        thread.Stack.AddClient(connection);
                        IndexConnection(id, connection);
                        return true;
                    }
                }

                if(MaxCount == 0 || Threads.Count < MaxCount) {
                    CreateThread(connection);
                    IndexConnection(id, connection);
                    return true;
                }
            }

            return false;
        }

        public void InvalidateThread(Stack<T> stackRef) {
            lock(Threads) {
                var ctx = Threads.FirstOrDefault(x => Object.ReferenceEquals(x.Stack, stackRef));
                if(ctx != null) {
                    Threads.Remove(ctx);
                    updateFullThreadCount = true;
                }
            }
        }

        private ThreadContext CreateThread(Connection initialConnection = null, bool runWithNoClients = false) {
            var stack = new Stack<T>(this, runWithNoClients, initialConnection);
            var ctx = new ThreadContext {
                Stack = stack,
                Thread = new Thread(new ThreadStart(stack.ManageStack))
            };
            
            Threads.Add(ctx);
            updateFullThreadCount = true;
            return ctx;
        }

        private int FullThreadSize {
            get {
                if(updateFullThreadCount) {
                    _fullThreadCount = Math.Min(
                            MaxSize == 0 ? int.MaxValue : MaxSize, 
                            InitialSize + SizeGrowth * (Threads.Count - InitialCount)
                        );
                }

                return _fullThreadCount;
            }
        }

        class ThreadContext {
            public Thread Thread { get; set; }
            public Stack<T> Stack { get; set; }
        }
    }
}
