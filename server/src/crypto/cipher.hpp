#ifndef SOSC_CRYPTO_CIPHER_H
#define SOSC_CRYPTO_CIPHER_H

#include "../utils/bigint.hpp"
#include "keyex.hpp"

namespace sosc {
namespace cgc {
class Cipher {
public:
    Cipher() {};
    Cipher(const KeyExchange& key);
    
    void Parse(std::string* data);
private:
    std::string GenerateStream(uint64_t length);
    
    const int state_size = 256;
    std::string state;
};
}}

#endif
