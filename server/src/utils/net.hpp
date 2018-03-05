#ifndef SOSC_UTIL_NET_H
#define SOSC_UTIL_NET_H

#include <sstream>
#include <algorithm>
#include <string>
#include <cstring>

#undef HTONS
#undef HTONUS
#undef NTOHS
#undef NTOHUS

#undef HTONL
#undef HTONUL
#undef NTOHL
#undef NTOHUL

#undef HTONLL
#undef HTONULL
#undef NTOHLL
#undef NTOHULL

#define HTONS (X) sosc::net::htonv<int16_t>(X)
#define HTONUS(X) sosc::net::htonv<uint16_t>(X)
#define NTOHS (X) sosc::net::ntohv<int16_t>(X)
#define NTOHUS(X) sosc::net::ntohv<uint16_t>(X)

#define HTONL (X) sosc::net::htonv<int32_t>(X)
#define HTONUL(X) sosc::net::htonv<uint32_t>(X)
#define NTOHL (X) sosc::net::ntohv<int32_t>(X, 0)
#define NTOHUL(X) sosc::net::ntohv<uint32_t>(X, 0)

#define HTONLL (X) sosc::net::htonv<int64_t>(X)
#define HTONULL(X) sosc::net::htonv<uint64_t>(X)
#define NTOHLL (X) sosc::net::ntohv<int64_t>(X, 0)
#define NTOHULL(X) sosc::net::ntohv<uint64_t>(X, 0)

namespace sosc {
namespace net {    
class IpAddress {
public:
    
private:
    typedef std::pair<uint16_t, uint8_t> addrpart_t;
    addrpart_t parts[8] = {};
    
    static addrpart_t ParsePart(std::string part);
};

bool is_big_endian();
template<typename T>
std::string htonv(T host_var) {
    int byte_count = sizeof(T);
    std::string net_var(byte_count, 0);
    
    for(int b = 0; b < byte_count; ++b) {
        int i = is_big_endian()
            ? b : byte_count - b - 1;
            
        net_var[i] = ((uint8_t*)&host_var)[b];
    }
    
    return net_var;
}
template<typename T>
T ntohv(std::string net_var, size_t offset = 0) {
    int byte_count = sizeof(T);
    T host_var = 0;
    
    for(int b = 0; b < byte_count; ++b) {
        if(offset + b >= net_var.length())
            break;
        
        int i = is_big_endian()
            ? b : byte_count - b - 1;
            
        ((uint8_t*)&host_var)[i] = net_var[offset + b];
    }
    
    return host_var;
}
}}

#endif
