#ifndef SOSC_UTIL_CSPRNG_H
#define SOSC_UTIL_CSPRNG_H

#include <fstream>
#include <string>
#include <mutex>
#include <random>

namespace sosc {
namespace csprng {
std::string next_bytes(std::string::size_type count);

template<typename T>
T next() {
    return *((T*)(next_bytes(sizeof(T)).c_str()));
}
}}

#endif
