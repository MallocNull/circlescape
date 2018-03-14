#include "sha1.hpp"

#define SHA1_BLOCK_SIZE 64

static inline uint32_t Z(uint32_t x, uint32_t y) {
    return x + y;
}

static inline uint32_t S(uint32_t X, uint8_t n) {
    n = n % 32;
    return (X << n) | (X >> (32 - n));
}

static uint32_t f
    (uint8_t t, uint32_t B, uint32_t C, uint32_t D)
{
    t = t % 80;
    if(t <= 19)
        return (B & C) | (~B & D);
    else if(t >= 20 && t <= 39)
        return B ^ C ^ D;
    else if(t >= 40 && t <= 59)
        return (B & C) | (B & D) | (C & D);
    else
        return B ^ C ^ D;
}

static uint32_t K(uint8_t t) {
    t = t % 80;
    if(t <= 19)
        return 0x5A827999;
    else if(t >= 20 && t <= 39)
        return 0x6ED9EBA1;
    else if(t >= 40 && t <= 59)
        return 0x8F1BBCDC;
    else
        return 0xCA62C1D6;
}

std::string sosc::cgc::sha1(const std::string& data) {
    int last_block = data.length() % 64, 
        pad_size = SHA1_BLOCK_SIZE - last_block;
        
    if(last_block >= 55)
        pad_size += SHA1_BLOCK_SIZE;
    
    std::string padding(0, pad_size);
    padding[0] = 0x80;
    
    for(int i = 0; i < 8; ++i) {
        padding[padding.length() - i - 1] =
            (padding.length() >> (8 * i)) & 0xFF;
    }
           
    uint32_t H[5] = {
        0x67452301,
        0xEFCDAB89,
        0x98BADCFE,
        0x10325476,
        0xC3D2E1F0
    }, W[80], A, B, C, D, E;
    
    for(int i = 0; i < data.length() + padding.length(); i += SHA1_BLOCK_SIZE) 
    {
        for(int j = 0; j < 16; ++j) {
            W[j] = 0;
            for(int k = 0; k < 4; ++k) {
                if(i + j + k < data.length())
                    W[j] |= (data[i + j + k]
            }
        }
    }
}
