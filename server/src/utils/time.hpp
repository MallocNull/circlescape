#ifndef SOSC_UTIL_TIME_H
#define SOSC_UTIL_TIME_H

#include <sstream>
#include <algorithm>
#include <string>
#include <cstring>
#include <chrono>
#include <ctime>
#include <mutex>

namespace sosc {
typedef std::chrono::system_clock clock;
typedef std::chrono::time_point<sosc::clock> time;

namespace clk {
sosc::time from_unix_time(uint64_t unix);

std::tm to_utc(sosc::time time);
std::tm to_utc(const time_t* time);

std::tm to_local(sosc::time time);
std::tm to_local(const time_t* time);

std::tm to_tm(sosc::time time);
std::tm to_tm(const time_t* time);

sosc::time error_time();
bool is_error_time(std::string data, size_t offset = 0);
bool is_error_time(sosc::time time);

sosc::time unpack_time(std::string data, size_t offset = 0);

std::string pack_time();
std::string pack_time(sosc::time time);
std::string pack_error_time();
}}

#endif
