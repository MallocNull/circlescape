#include "bigint.hpp"

static bool is_hex_char(char c) {
    return ((c >= 'a') && (c <= 'f'))
        || ((c >= 'A') && (c <= 'F'))
        || ((c >= '0') && (c <= '9'));
}

sosc::BigUInt::BigUInt() {
    this->value = { 0 };
}

sosc::BigUInt::BigUInt(std::string hex_str, int byte_count) {
    this->Parse(hex_str, byte_count);
}

bool sosc::BigUInt::Parse(std::string hex_str, int byte_count) {
    if(hex_str.length() == 0)
        hex_str = "00";
    if(hex_str.length() % 2 != 0)
        hex_str.insert(hex_str.begin(), '0');
    
    int str_byte_count = hex_str.length() / 2;
    byte_count = std::max(byte_count, str_byte_count);
    
    this->value = std::vector<uint8_t>(byte_count, 0);
    for(int i = 0; i < hex_str.length(); i += 2) {
        if(!is_hex_char(hex_str[i]) || !is_hex_char(hex_str[i + 1]))
            return false;
        
        this->value[str_byte_count - (i / 2) - 1] 
            = std::stoi(hex_str.substr(i, 2), 0, 16);
    }
    
    return true;
}

void sosc::BigUInt::Random(int byte_count) {
    this->value = std::vector<uint8_t>(byte_count, 0);
    std::string random_str = csprng::next_bytes(byte_count);
    for(int i = 0; i < byte_count; ++i)
        this->value[i] = random_str[i];
}

void sosc::BigUInt::RandomPrime(int byte_count) {
    do {
        this->Random(byte_count);
        this->value[0] |= 0x01;
        this->value[byte_count - 1] |= 0x80;
    } while(!this->IsProbablePrime());
}

bool sosc::BigUInt::IsZero() const {
    return
        std::all_of(this->value.begin(), this->value.end(), 
            [](uint8_t x) { return x == 0; });
}

bool sosc::BigUInt::IsOne() const {
    if(this->value[0] != 1)
        return false;
    
    return this->value.size() == 1
        ? true
        : std::all_of(this->value.begin() + 1, this->value.end(),
            [](uint8_t x) { return x == 0; });
}

bool sosc::BigUInt::IsEven() const {
    return !this->GetBit(0);
}

bool sosc::BigUInt::IsProbablePrime() const {
    // TODO rabin-miller
    return false;
}

std::tuple<sosc::BigUInt, sosc::BigUInt> sosc::BigUInt::DivideWithRemainder
    (const BigUInt& num, const BigUInt& denom)
{
    if(num.IsZero())
        return std::make_tuple(BigUInt(), BigUInt());
    if(denom.IsZero())
        throw "BigUInt division by zero";
    if(denom.IsOne())
        return std::make_tuple(num, BigUInt());
    if(denom > num)
        return std::make_tuple(BigUInt(), num);
    
    BigUInt quotient, remainder;
    for(uint64_t i = num.ByteCount() * 8 - 1;; --i) {
        remainder = remainder << 1;
        remainder.SetBit(0, num.GetBit(i));
        
        if(remainder >= denom) {
            remainder -= denom;
            quotient.SetBit(i, true);
        }
        
        if(i == 0)
            break;
    }
    
    return std::make_tuple(quotient, remainder);
}

sosc::BigUInt sosc::BigUInt::ModPow
    (const BigUInt& base, const BigUInt& exp, const BigUInt& mod)
{
    BigUInt accum("1");
    BigUInt x = exp;
    BigUInt bpow = base;
    
    while(!x.IsZero()) {
        if(!x.IsEven())
            accum = (accum * bpow) % mod;
        
        x = x >> 1;
        bpow = (bpow * bpow) % mod;
    }
    
    return accum;
}

void sosc::BigUInt::SetBit(uint64_t bit, bool value) {
    uint64_t byte = bit / 8;
    if(byte >= this->ByteCount())
        this->value.resize(byte + 1);
    
    if(value)
        this->value[byte] |= (1 << (bit % 8));
    else
        this->value[byte] &= ~(1 << (bit % 8));
}

bool sosc::BigUInt::GetBit(uint64_t bit) const {
    uint64_t byte = bit / 8;
    if(byte >= this->ByteCount())
        return false;
    
    return (this->value[byte] & (1 << (bit % 8))) != 0;
}

sosc::BigUInt sosc::BigUInt::operator + (const BigUInt& rhs) const {
    uint64_t sum_range = std::min(this->ByteCount(), rhs.ByteCount());
    
    BigUInt sum;
    uint8_t carry = 0;
    for(uint64_t i = 0; i < sum_range; ++i) {
        
    }
    
    return sum;
}

sosc::BigUInt& sosc::BigUInt::operator += (const BigUInt& rhs) {
    this->Copy(*this + rhs);
    return *this;
}
