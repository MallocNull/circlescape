#ifndef SOSC_UTIL_NET_H
#define SOSC_UTIL_NET_H

#include <sstream>
#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <cstring>

#undef htons
#undef HTONS
#undef htonus
#undef HTONUS
#undef ntohs
#undef NTOHS
#undef ntohus
#undef NTOHUS

#undef htonl
#undef HTONL
#undef htonul
#undef HTONUL
#undef ntohl
#undef NTOHL
#undef ntohul
#undef NTOHUL

#undef htonll
#undef HTONLL
#undef htonull
#undef HTONULL
#undef ntohll
#undef NTOHLL
#undef ntohull
#undef NTOHULL

#define HTONS (X) sosc::net::htonv<int16_t>(X)
#define HTONUS(X) sosc::net::htonv<uint16_t>(X)
#define NTOHS (X) sosc::net::ntohv<int16_t>(X)
#define NTOHUS(X) sosc::net::ntohv<uint16_t>(X)

#define HTONL (X) sosc::net::htonv<int32_t>(X)
#define HTONUL(X) sosc::net::htonv<uint32_t>(X)
#define NTOHL (X) sosc::net::ntohv<int32_t>(X)
#define NTOHUL(X) sosc::net::ntohv<uint32_t>(X)

#define HTONLL (X) sosc::net::htonv<int64_t>(X)
#define HTONULL(X) sosc::net::htonv<uint64_t>(X)
#define NTOHLL (X) sosc::net::ntohv<int64_t>(X)
#define NTOHULL(X) sosc::net::ntohv<uint64_t>(X)

namespace sosc {
typedef std::chrono::system_clock clock;
typedef std::chrono::time_point<sosc::clock> time;

namespace net {    
class IpAddress {
public:
    
private:
    
};

bool is_big_endian();
template<typename T = uint32_t>
std::string htonv(T host_var);
template<typename T = uint32_t>
T ntohv(std::string net_var, size_t offset = 0);

std::tm to_utc(const time_t* time);
bool is_error_time(std::string data, size_t offset = 0);
bool is_error_time(sosc::time time);
std::string pack_time();
std::string pack_time(sosc::time time);
std::string pack_error_time();
sosc::time unpack_time(std::string data, size_t offset = 0);
}}

#endif
