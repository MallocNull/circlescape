#include "csprng.hpp"

#ifdef _WIN32
// TODO update win32 code to use csprng in advapi32.lib
// NOTE RUNNING ON WINDOWS IS INSECURE DO NOT DOD !!!

#include <ctime>

static struct __rng_ctx {
    std::default_random_engine rng;
    std::mutex rng_mtx;
    
    __rng_ctx() : rng(time(NULL)) { }
} _rng_ctx;

std::string sosc::csprng::next_bytes(std::string::size_type count) {
    _rng_ctx.rng_mtx.lock();
    
    std::string rand_bytes;
    for(std::string::size_type i = 0; i < count; ++i)
        rand_bytes += (char)_rng_ctx.rng();
    
    _rng_ctx.rng_mtx.unlock();
    return rand_bytes;
}

#else

static struct __rng_ctx {
    std::ifstream urand;
    std::mutex rng_mtx;
    
    __rng_ctx() { 
        this->urand.open("/dev/urandom", 
            std::ifstream::in | std::ifstream::binary);
        
        if(!this->urand.good())
            throw "Could not establish csprng context.";
    }
    
    ~__rng_ctx() {
        this->urand.close();
    }
} _rng_ctx;

std::string sosc::csprng::next_bytes(std::string::size_type count) {
    _rng_ctx.rng_mtx.lock();
    
    char* buffer = new char[count];
    _rng_ctx.urand.read(buffer, count);
    std::string rand_bytes(buffer, count);
    delete[] buffer;
    
    _rng_ctx.rng_mtx.unlock();
    return rand_bytes;
}

#endif
