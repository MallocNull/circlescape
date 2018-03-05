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

}}


#endif
