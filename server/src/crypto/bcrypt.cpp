#include "bcrypt.hpp"

static const uint32_t bcrypt_ciphertext[6] = {
    0x4f727068, 0x65616e42, 0x65686f6c,
    0x64657253, 0x63727944, 0x6f756274
};

std::string sosc::cgc::bcrypt_hash(const std::string& input) {
    std::string salt = csprng::next_bytes(16),
        input_t = input.substr(0, 72);
        
    uint32_t ctext[6];
    std::memcpy(ctext, bcrypt_ciphertext, 6 * sizeof(uint32_t));
    
    Blowfish bfish;
    bfish.SetEksKey(salt, input);
    for(int i = 0; i < BCRYPT_COST; ++i) {
        bfish.SetKey(input.c_str(), input.length());
        bfish.SetKey(salt.c_str(), salt.length());
    }
    
    for(int i = 0; i < 64; ++i)
        for(int j = 0; j < 6; j += 2)
            bfish.EncryptBlock(&ctext[j], &ctext[j + 1]);
        
    std::string ct_hash(24, 0);
    for(int i = 0; i < 6; ++i) {
        for(int j = 0; j < 4; ++j) {
            ct_hash[i * 4 + j] = ctext[i] & 0xFF;
            ctext[i] >>= 8;
        }
    }
    
    std::stringstream stream;
    stream << "$2b$" 
           << (BCRYPT_COST < 10 ? "0" : "") << BCRYPT_COST
           << "$" << base64_encode(salt, true)
           << base64_encode(ct_hash, true);
           
    return stream.str();
}
