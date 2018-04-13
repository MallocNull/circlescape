#ifndef SOSC_POOL_H
#define SOSC_POOL_H

#include <numeric>
#include <vector>
#include <list>
#include <thread>
#include <mutex>

namespace sosc {
typedef struct {
    // amount of threads to start with (never close)
    int initial_count = 3;
    // starting limit of clients per thread
    int initial_size = 3;
    // amount the limit scales when threshold is passed
    int size_growth = 1;
    
    // any values below marked -1 indicates no limit
    // max amount of clients in a scaled thread
    int max_size = -1;
    // maximum number of threads
    int max_count = -1;
    // maximum number of connections in the pool
    int max_total = -1;
    // excess deviance from threshold necessary to rebalance
    int tolerance = -1;
} poolinfo_t;
    
template<class T>
class Pool {
public:
    Pool(const poolinfo_t& info);
    
    void Start();
    bool AddClient(T* client);
    int ClientCount() const;
    
    void Stop();
protected:
    virtual void ProcessClient(T* client) = 0;
private:
    class Stack {
    public:
        Stack(Pool<T> *pool);
        void Start();
        
        void AddClient(T* client);
        int ClientCount() const;
        
        void Stop();
    private:
        std::thread thread;
        Pool<T> *pool;
        bool is_open;
        
        std::list<T*> clients;
        std::mutex clients_mtx;
    };
    
    poolinfo_t info;
    bool is_running;
    std::vector<Stack> stacks;
    friend class Stack;
};

template<class T>
Pool<T>::Pool(const poolinfo_t& info) {
    this->info = info;
    this->is_running = false;
}

template<class T>
void Pool<T>::Start() {
    if(this->is_running == true)
        return;
    
    for(int i = 0; i < this->info.initial_count; ++i) {
        this->stacks.push_back(Stack(this));
        this->stacks.back().Start();
    }
    
    this->is_running = true;
}

template<class T>
bool Pool<T>::AddClient(T* client) {
    
    
    return false;
}

template<class T>
int Pool<T>::ClientCount() const {
    int count = 0;
    for(auto i = this->stacks.begin(); i != this->stacks.end(); ++i)
        count += i->ClientCount();
    return count;
}
}

#endif
