#ifndef SOSC_CRYPTO_SHA1
#define SOSC_CRYPTO_SHA1

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace sosc {
namespace cgc {
std::string sha1(const std::string& data, bool raw = false);
}}

#endif
