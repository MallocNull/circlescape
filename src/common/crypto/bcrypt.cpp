#include "bcrypt.hpp"
#include "bfish.hpp"

static const uint32_t bcrypt_ciphertext[6] = {
    0x4f727068, 0x65616e42, 0x65686f6c,
    0x64657253, 0x63727944, 0x6f756274
};

std::string sosc::cgc::bcrypt_hash_raw
    (const std::string& input, const std::string& salt, uint32_t cost) 
{
    std::string salt_t = salt.substr(0, 16),
                input_t = input.substr(0, 72);
        
    uint32_t ctext[6];
    std::memcpy(ctext, bcrypt_ciphertext, 6 * sizeof(uint32_t));
    
    Blowfish bfish;
    bfish.SetEksKey(salt_t, input_t);
    for(uint32_t i = 0; i < 1 << (cost % 32); ++i) {
        bfish.SetKey(input_t.c_str(), input_t.length());
        bfish.SetKey(salt_t.c_str(), salt_t.length());
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
           << (cost < 10 ? "0" : "") << cost
           << "$" << base64_encode(salt_t, true)
           << base64_encode(ct_hash, true);
           
    return stream.str();
}

std::string sosc::cgc::bcrypt_hash(const std::string& input) {
    return bcrypt_hash_raw(input, sosc::csprng::next_bytes(16), BCRYPT_COST);
}

bool sosc::cgc::bcrypt_check
    (const std::string& input, const std::string& hash) 
{
    if(hash.length() != 60 && !str::starts(hash, "$2b$"))
        return false;
    
    auto parts = str::split(hash, '$');
    if(parts.size() != 4)
        return false;
    parts.erase(parts.begin());
    
    if(parts[1].length() != 2)
        return false;
    
    int cost;
    try {
        cost = std::stoi(parts[1], 0, 10);
    } catch(...) {
        return false;
    }
    
    std::string salt = parts[2].substr(0, 22),
        input_t = input.substr(0, 72),
        hash_chk = bcrypt_hash_raw(input_t, base64_decode(salt, true), cost);
    
    // timing safe compare DO NOT OPTIMIZE
    bool success = true;
    for(int i = 0; i < hash.length(); ++i)
        if(hash[i] != hash_chk[i])
            success = false;
    
    return success;
}
