#ifndef SOSC_CRYPTO_BCRYPT_H
#define SOSC_CRYPTO_BCRYPT_H

#include <sstream>
#include <cstring>
#include <string>
#include "base64.hpp"
#include "bfish.hpp"
#include "../utils/csprng.hpp"

#define BCRYPT_COST 10

namespace sosc {
namespace cgc {
std::string bcrypt_hash(const std::string& input);
bool bcrypt_check(std::string input, std::string hash);
}}

#endif
