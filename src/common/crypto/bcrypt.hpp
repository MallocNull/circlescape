#ifndef SOSC_CRYPTO_BCRYPT_H
#define SOSC_CRYPTO_BCRYPT_H

#include <sstream>
#include <cstring>
#include <string>
#include "base64.hpp"
#include "utils/csprng.hpp"
#include "utils/string.hpp"

#define BCRYPT_COST 10

namespace sosc {
namespace cgc {
std::string bcrypt_hash(const std::string& input);
std::string bcrypt_hash_raw
    (const std::string& input, const std::string& salt, uint32_t cost);

bool bcrypt_check(const std::string& input, const std::string& hash);
}}

#endif
