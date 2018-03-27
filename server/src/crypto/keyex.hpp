#ifndef SOSC_CRYPTO_KEYEX_H
#define SOSC_CRYPTO_KEYEX_H

#include "../utils/bigint.hpp"
#include "../sock/packet.hpp"

namespace sosc {
namespace cgc {
class KeyExchange {
public:
    KeyExchange();
    
    Packet GenerateRequest() const;
    bool ParseRequest(const Packet& request, Packet* response);
    bool ParseResponse(const Packet& response);
    
    inline bool Succeeded() {
        return !this->private_key.IsZero();
    }
    
    inline const BigUInt& GetPrivateKey() {
        return this->private_key;
    }
private:
    const int key_size = 256;
    const int key_size_bytes = key_size / 8;
    
    const BigUInt generator = BigUInt(2u);
    static BigUInt secret;
    BigUInt modulus;
    
    BigUInt private_key;
};
}}

#endif
