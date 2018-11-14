#ifndef SOSC_UTIL_NET_H
#define SOSC_UTIL_NET_H

#include <utility>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstring>
#include "string.hpp"

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

#define  HTONS(X) sosc::net::htonv<int16_t>(X)
#define HTONUS(X) sosc::net::htonv<uint16_t>(X)
#define  NTOHS(X) sosc::net::ntohv<int16_t>(X)
#define NTOHUS(X) sosc::net::ntohv<uint16_t>(X)

#define  HTONL(X) sosc::net::htonv<int32_t>(X)
#define HTONUL(X) sosc::net::htonv<uint32_t>(X)
#define  NTOHL(X) sosc::net::ntohv<int32_t>(X, 0)
#define NTOHUL(X) sosc::net::ntohv<uint32_t>(X, 0)

#define  HTONLL(X) sosc::net::htonv<int64_t>(X)
#define HTONULL(X) sosc::net::htonv<uint64_t>(X)
#define  NTOHLL(X) sosc::net::ntohv<int64_t>(X, 0)
#define NTOHULL(X) sosc::net::ntohv<uint64_t>(X, 0)

namespace sosc {
namespace net {    
class IpAddress {
public:
    // all strings taken as arguments will accept both IPv4 and IPv6 addrs
    // IPv4 addrs will be stored as IPv6 in this format:
    //      127.126.0.255
    //   -> ::FFFF:7F7E:FF
    //          ^  ^ ^ ^
    //          +--|-|-|----- UNIQUE PREFIX VALUE (decimal 65535)
    //             +-|-|----- FIRST OCTET AS HEXADECIMAL (127 -> 7F)
    //               +-|----- SECOND OCTET AS HEXADECIMAL (126 -> 7E)
    //                 +---+- THIRD OCTET IS HIDDEN SINCE FOURTH IS LOWER
    //                     +- (0.255 -> 00FF -{equiv. to}-> FF)
    
    IpAddress();
    bool Parse(const std::string& addr);
    
    operator std::string () const;
    std::string ToString(bool force_ipv6 = false) const;
    
    bool operator == (const IpAddress& rhs) const;
    bool operator != (const IpAddress& rhs) const;
    
    bool IsIdentical(const IpAddress& rhs) const;
    bool IsIPv4() const;
private:
    // pair item 1 is the two bytes in an IPv6 address
    // pair item 2 is a wildcard bitmask on the nibble in the address
    //  4 msbits unused, 4 lsbits indicate wildcards if bit is set
    //  ex. ::F F F F (IPv6 address)
    //        0 1 1 0 (bitmask)
    //        4 3 2 1 (bit position where 1 is LSB)
    //      would treat the middle two nibbles as wildcards; so this ip 
    //      would match eg. ::F00F, ::FAAF, ::FEAF etc.
    
    typedef std::pair<uint16_t, uint8_t> addrpart_t;
    std::string tostr_cache;
    bool tostr_cache_inval = true;
    addrpart_t parts[8];
    
    void Reset();
    bool ParseError();
    bool ParseIPv4Parts(const std::vector<std::string>& parts);
    int ParseIPv6Part(const std::string& addr_part, bool from_start);
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
            
        ((uint8_t*)&host_var)[i] = (uint8_t)net_var[offset + b];
    }
    
    return host_var;
}
}}

#endif
