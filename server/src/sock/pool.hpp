#ifndef SOSC_POOL_H
#define SOSC_POOL_H

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
    inline bool IsOpen() const {
        return this->is_open;
    }
    
    void Stop();
protected:
    virtual bool ProcessClient(T* client) = 0;
private:
    bool IsStackFull(int stackCount) const;
    bool CanAddStack() const;
    
    class Stack {
    public:
        Stack(Pool<T>* pool);
        void Start();
        
        void AddClient(T* client);
        int ClientCount() const;
        inline bool IsOpen() const {
            return this->is_open;
        }
        
        void Stop();
    private:
        void StackThread();
        
        std::thread thread;
        Pool<T> *pool;
        bool is_open;
        bool is_running;
        
        std::list<T*> clients;
        std::mutex clients_mtx;
    };
    
    poolinfo_t info;
    bool is_open;
    
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
    if(this->is_running)
        return;
    
    for(int i = 0; i < this->info.initial_count; ++i) {
        this->stacks.push_back(Stack(this));
        this->stacks.back().Start();
    }
    
    this->is_running = true;
}

template<class T>
bool Pool<T>::IsStackFull(int stackCount) const {
    poolinfo_t *info = &this->info;
    return info->max_size != -1 
        && stackCount < 
            info->initial_size 
                + (info->size_growth 
                    * (this->stacks.size() - info->initial_count))
        && stackCount < info->max_size;
}

template<class T>
bool Pool<T>::CanAddStack() const {
    return this->info.max_count == -1 
        || this->stacks.size() < this->info.max_count;
}

template<class T>
bool Pool<T>::AddClient(T* client) {
    if(!this->is_running)
        return;
    
    if(this->info.max_total != -1)
       if(this->ClientCount() >= this->info.max_total)
           return false;
    
    int lowestCount = -1;
    Stack* lowestStack = nullptr;
    for(auto i = this->stacks.begin(); i != this->stacks.end(); ++i) {
        int thisCount;
        if((thisCount = i->ClientCount()) > lowestCount) {
            lowestCount = thisCount;
            lowestStack = &(*i);
        }
    }
    
    if(lowestStack != nullptr && !this->IsStackFull(lowestCount))
        lowestStack->AddClient(client);
    else if(this->CanAddStack()) {
        this->stacks.push_back(Stack(this));
        this->stacks.back().AddClient(client);
    } else
        return false;
    
    return true;
}

template<class T>
int Pool<T>::ClientCount() const {
    if(!this->is_running)
        return 0;
    
    int count = 0;
    for(auto i = this->stacks.begin(); i != this->stacks.end(); ++i)
        count += i->ClientCount();
    return count;
}

template<class T>
void Pool<T>::Stop() {
    if(!this->is_running)
        return;
    
    for(auto i = this->stacks.begin(); i != this->stacks.end(); ++i)
        i->Stop();
    
    stacks->clear();
    this->is_running = false;
}

template<class T>
Pool<T>::Stack::Stack(Pool<T>* pool) {
    this->pool = pool;
    this->is_open = false;
    this->is_running = false;
}

template<class T>
void Pool<T>::Stack::Start() {
    if(this->is_open || this->is_running)
        return;
    
    this->is_open = true;
    this->is_running = true;
    this->thread = std::thread(this->StackThread, this);
}

template<class T>
void Pool<T>::Stack::AddClient(T* client) {
    if(!this->is_open || !this->is_running)
        return;
    
    this->clients_mtx.lock();
    this->clients.push_back(client);
    this->clients_mtx.unlock();
}

template<class T>
int Pool<T>::Stack::ClientCount() const {
    if(!this->is_open || !this->is_running)
        return 0;
    
    this->clients_mtx.lock();
    int count = this->clients.size();
    this->clients_mtx.unlock();
    
    return count;
}

template<class T>
void Pool<T>::Stack::StackThread() {
    while(this->is_running) {
        for(auto i = this->clients.begin(); i != this->clients.end(); ++i) {
            if(!this->is_running)
                break;
            
            this->clients_mtx.lock();
            if(!this->pool->ProcessClient(*i))
                this->clients.erase(i);
            this->clients_mtx.unlock();
        }
    }
}

template<class T>
void Pool<T>::Stack::Stop() {
    if(!this->is_open || !this->is_running)
        return;
    
    this->is_running = false;
    this->thread.join();
    this->is_open = false;
}
}

#endif
