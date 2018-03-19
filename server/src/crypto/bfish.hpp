#ifndef SOSC_CRYPTO_BFISH_H
#define SOSC_CRYPTO_BFISH_H

#include <string>
#include <cstring>

namespace sosc {
namespace cgc {
class Blowfish {
public:
    Blowfish(const std::string& key);
    
    std::string Encrypt(std::string data) const;
    std::string Decrypt(const std::string& data) const;
private:
    void SetKey(const char* key, size_t length);
    
    void EncryptBlock(uint32_t* left, uint32_t* right) const;
    void DecryptBlock(uint32_t* left, uint32_t* right) const;
    uint32_t Feistel(uint32_t input) const;
    
    uint32_t parr[18];
    uint32_t sbox[4][256];
};
}}

#endif
