#include "sha1.hpp"

#define SHA1_BLOCK_SIZE 64

#undef A
#define A 0
#undef B
#define B 1
#undef C
#define C 2
#undef D
#define D 3
#undef E
#define E 4

static inline uint32_t Z(uint32_t x, uint32_t y) {
    return x + y;
}

static inline uint32_t S(uint32_t X, uint8_t n) {
    n = n % 32;
    return (X << n) | (X >> (32 - n));
}

static uint32_t f
    (uint8_t t, uint32_t b, uint32_t c, uint32_t d)
{
    t = t % 80;
    if(t <= 19)
        return (b & c) | (~b & d);
    else if(t >= 20 && t <= 39)
        return b ^ c ^ d;
    else if(t >= 40 && t <= 59)
        return (b & c) | (b & d) | (c & d);
    else
        return b ^ c ^ d;
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
    }, W[80], T, O[5];
    
    for(int i = 0; i < data.length() + padding.length(); i += SHA1_BLOCK_SIZE) 
    {
        for(int j = 0; j < 16; ++j) {
            W[j] = 0;
            for(int k = 0; k < 4; ++k) {
                W[j] |= (((i + j + k < data.length()) 
                    ? data : padding)[i + j + k] >> (8 * (3 - i))) & 0xFF;
            }
        }
        
        for(int j = 16; j < 80; ++j)
            W[j] = S(W[j - 3] ^ W[j - 8] ^ W[j - 14] ^ W[j - 16], 1);
        
        for(int j = 0; j < 5; ++j)
            O[j] = H[j];
        
        for(int j = 0; j < 80; ++j) {
            T = S(O[A], 5) + f(j, O[B], O[C], O[D]) + O[E] + W[j] + K(j); 
            O[E] = O[D];
            O[D] = O[C];
            O[C] = S(O[B], 30);
            O[B] = O[A];
            O[A] = T;
        }
        
        for(int j = 0; j < 5; ++j)
            H[j] += O[j];
    }
    
    return "";
}
