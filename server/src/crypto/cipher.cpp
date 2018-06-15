#include "cipher.hpp"

template<typename T>
static void _swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

sosc::cgc::Cipher::Cipher(const KeyExchange& key) {
    std::string key_raw 
        = key.GetPrivateKey().ToRawString((uint64_t)key.key_size_bytes);

    for(int i = 0; i < this->state_size; ++i)
        this->state[i] = (uint8_t)i;
    
    int i = 0, j = 0;
    for(i = 0; i < this->state_size; ++i)
        j = (j + this->state[i] + key_raw[i % key_raw.length()]) % 256;
    
    GenerateStream(1024);
}

void sosc::cgc::Cipher::Parse
    (std::string* data, std::string::size_type offset)
{
    std::string stream = this->GenerateStream(data->length() - offset);
    for(std::string::size_type i = offset; i < data->length(); ++i)
        (*data)[i] ^= stream[i];
}

std::string sosc::cgc::Cipher::GenerateStream(uint64_t length) {
    std::string stream(length, 0);
    int i = 0, j = 0;
    
    for(uint64_t x = 0; x < length; ++x) {
        i = (i + 1) % 256;
        j = (j + this->state[i]) % 256;
        _swap(this->state[i], this->state[j]);
        
        stream[x] = 
            this->state[(this->state[i] + this->state[j]) % 256];
    }
    
    return stream;
}
