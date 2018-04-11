#ifndef SOSC_POOL_H
#define SOSC_POOL_H

#include <vector>
#include <list>
#include <thread>
#include <mutex>

namespace sosc {
template<typename T>
class Pool {
public:
    
protected:
    virtual void ProcessClient(T* client) = 0;
private:
    class Stack {
    public:
        
    private:
        std::thread thread;
        std::list<T*> clients;
        std::mutex clients_mtx;
        Pool<T> *pool;
        
        friend class Pool<T>;
    };
    
    std::vector<Stack> stacks;
    
    friend class Stack;
};
}

#endif
