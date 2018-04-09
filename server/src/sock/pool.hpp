#ifndef SOSC_POOL_H
#define SOSC_POOL_H

namespace sosc {
template<typename T>
class Pool {
public:
    
protected:
    virtual void ProcessClient(T* client) = 0;
private:
    
};
}

#endif
