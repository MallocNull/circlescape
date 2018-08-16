#ifndef SOSC_CRYPTO_CIPHER_H
#define SOSC_CRYPTO_CIPHER_H

#include "utils/bigint.hpp"
#include "keyex.hpp"

namespace sosc {
namespace cgc {
class Cipher {
public:
    Cipher() = default;
    explicit Cipher(const KeyExchange& key);
    
    void Parse(std::string* data, std::string::size_type offset = 0);
private:
    std::string GenerateStream(uint64_t length);
    
    static const int state_size = 256;
    uint8_t state[state_size];
};
}}

#endif
