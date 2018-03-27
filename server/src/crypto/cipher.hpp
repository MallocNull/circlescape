#ifndef SOSC_CRYPTO_CIPHER_H
#define SOSC_CRYPTO_CIPHER_H

#include "../utils/bigint.hpp"

namespace sosc {
namespace cgc {
class Cipher {
public:
    
private:
    const std::string sigma = "expand 32-byte k";
    const std::string tau = "expand 16-byte k";
    
    std::string key;
    std::string state;
};
}}

#endif
