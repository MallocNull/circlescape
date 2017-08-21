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
        // maximum amount of total connections in the pool
        // 0 means no limit
        public int MaxTotal { get; set; } = 0;
        // maximum number of connections in a thread that exceeds the calculated
        // amount for the pool's thread count before the connection redistribution
        // function is called
        // 0 means never redistribute
        public int Tolerance { get; set; } = 0;

        private bool Disposed = false;
        private int _fullThreadCount;
        private volatile bool updateFullThreadCount = true;

        private readonly List<ThreadContext> Threads
            = new List<ThreadContext>();

        private long InternalCounter = 0;
        private readonly Dictionary<UInt64, T> Connections
            = new Dictionary<UInt64, T>();

        public Pool() {
            for(var i = 0; i < InitialCount; ++i)
                CreateThread(runWithNoClients: true);
        }

        public T this[UInt64 id] {
            get {
                lock (Connections) {
                    if (HasConnection(id))
                        return Connections[id];
                    else return null;
                }
            }
        }

        public bool HasConnection(UInt64 id) => Connections.ContainsKey(id);

        private void IndexConnection(T connection) {
            lock(Connections) {
                if(connection.IsIdNull)
                    connection.Id = (ulong)Interlocked.Increment(ref InternalCounter);
                
                Connections.Add(connection.Id, connection);
            }
        }

        internal void InvalidateConnection(UInt64 id) {
            lock(Connections) {
                Connections.Remove(id);
            }
        }

        public void Broadcast(byte[] message) {
            if(Disposed)
                return;

            lock(Connections) {
                foreach(var conn in Connections)
                    conn.Value.Send(message);
            }
        }

        public bool AddConnection(T connection) {
            if(Disposed)
                return false;

            if(MaxTotal != 0 && Connections.Count >= MaxTotal)
                return false;

            lock(Threads) {
                foreach(var thread in Threads) {
                    if(thread.Stack.Count < FullThreadSize) {
                        thread.Stack.AddClient(connection);
                        IndexConnection(connection);
                        return true;
                    }
                }

                if(MaxCount == 0 || Threads.Count < MaxCount) {
                    CreateThread(connection);
                    IndexConnection(connection);
                    return true;
                }
            }

            return false;
        }

        internal void InvalidateThread(Stack<T> stackRef) {
            lock(Threads) {
                var ctx = Threads.FirstOrDefault(x => ReferenceEquals(x.Stack, stackRef));
                if(ctx != null) {
                    Threads.Remove(ctx);
                    updateFullThreadCount = true;
                }
            }
        }

        private ThreadContext CreateThread(T initialConnection = null, bool runWithNoClients = false) {
            var stack = new Stack<T>(this, runWithNoClients, initialConnection);
            var ctx = new ThreadContext {
                Stack = stack,
                Thread = new Thread(stack.ManageStack)
            };

            ctx.Thread.Start();
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

        public void Dispose() {
            if(Disposed)
                return;

            Disposed = true;

            lock(Threads) {
                foreach(var thread in Threads)
                    thread.Stack.StopThread();
            }

            while(true) {
                Thread.Sleep(100);

                lock(Threads) {
                    if(Threads.Count == 0)
                        break;
                }
            }

            lock(Connections) {
                foreach(var conn in Connections) {
                    conn.Value.Disconnect(Frame.kClosingReason.Normal, "Server shutting down.");
                }
            }
        }

        ~Pool() {
            Dispose();
        }

        class ThreadContext {
            public Thread Thread { get; set; }
            public Stack<T> Stack { get; set; }
        }
    }
}
