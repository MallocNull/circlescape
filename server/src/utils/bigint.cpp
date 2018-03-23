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
    this->value = 
        std::vector<uint8_t>(std::max(byte_count, str_byte_count), 0);
        
    for(int i = 0; i < hex_str.length(); i += 2) {
        if(!is_hex_char(hex_str[i]) || !is_hex_char(hex_str[i + 1]))
            return false;
        
        this->value[str_byte_count - (i / 2) - 1] 
            = std::stoi(hex_str.substr(i, 2), 0, 16);
    }
    
    if(byte_count != -1)
        this->value.resize(byte_count, 0);
    return true;
}

void sosc::BigUInt::Random(int byte_count) {
    this->value = std::vector<uint8_t>(byte_count, 0);
    std::string random_str = csprng::next_bytes(byte_count);
    for(int i = 0; i < byte_count; ++i)
        this->value[i] = random_str[i];
}

sosc::BigUInt sosc::BigUInt::GenerateRandom(int byte_count) {
    BigUInt num;
    num.Random(byte_count);
    return num;
}

void sosc::BigUInt::RandomPrime(int byte_count) {
    do {
        this->Random(byte_count);
        this->value[0] |= 0x01;
        this->value[byte_count - 1] |= 0x80;
    } while(!this->IsProbablePrime());
}

sosc::BigUInt sosc::BigUInt::GenerateRandomPrime(int byte_count) {
    BigUInt num;
    num.RandomPrime(byte_count);
    return num;
}

size_t sosc::BigUInt::UsedByteCount() const {
    size_t ptr = this->ByteCount() - 1;
    for(;; --ptr) {
        if(this->value[ptr] != 0)
            break;
        if(ptr == 0)
            return 0;
    }
    
    return ptr + 1;
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

bool sosc::BigUInt::IsProbablePrime(uint16_t rounds) const {
    if(this->IsOne())
        return false;
    
    if    (*this == BigUInt(2u) 
        || *this == BigUInt(3u)
        || *this == BigUInt(5u))
    {
        return true;
    }
    
    if    (this->IsEven() 
        || this->IsDivisibleBy(BigUInt(3u))
        || this->IsDivisibleBy(BigUInt(5u)))
    {
        return false;
    }
    
    if(*this < BigUInt(25u))
        return true;
    
    for(uint16_t i = 0; i < rounds; ++i) {
        BigUInt rnd = BigUInt::GenerateRandom(this->ByteCount());
        rnd = (rnd < BigUInt(2u))
            ? BigUInt(2u)
            : (rnd > *this - BigUInt(2u))
                ? *this - BigUInt(2u)
                : rnd;
        
        if(BigUInt::ModPow(rnd, *this - BigUInt(1u), *this).IsOne())
            return false;
    }
    
    return true;
}

bool sosc::BigUInt::IsDivisibleBy(const BigUInt& value) const {
    if(value.IsZero())
        return false;
    if(value.IsOne())
        return true;
    if(value == BigUInt(2u))
        return value.IsEven();
    
    return (*this % value).IsZero();
}

sosc::division_t sosc::BigUInt::DivideWithRemainder
    (const BigUInt& num, const BigUInt& denom)
{
    if(num.IsZero())
        return division_t(BigUInt(), BigUInt());
    if(denom.IsZero())
        throw "BigUInt division by zero";
    if(denom.IsOne())
        return division_t(num, BigUInt());
    if(denom > num)
        return division_t(BigUInt(), num);
    
    BigUInt quotient, remainder;
    for(size_t i = num.ByteCount() * 8 - 1;; --i) {
        remainder = remainder << 1;
        remainder.SetBit(0, num.GetBit(i));
                  
        if(remainder >= denom) {
            remainder -= denom;
            quotient.SetBit(i, true);
        }
        
        if(i == 0)
            break;
    }
    
    return division_t(quotient, remainder);
}

sosc::BigUInt sosc::BigUInt::ModPow
    (const BigUInt& base, const BigUInt& exp, const BigUInt& mod)
{
    BigUInt accum("1");
    BigUInt x = exp;
    BigUInt bpow = base;
    
    uint64_t iterations = 0;
    while(!x.IsZero()) {
        ++iterations;
        
        if(!x.IsEven())
            accum = (accum * bpow) % mod;
        
        x = x >> 1;
        bpow = (bpow * bpow) % mod;
    }
    std::cout << std::endl << iterations << std::endl;
    
    return accum;
}

void sosc::BigUInt::SetBit(uint64_t bit, bool value) {
    size_t byte = bit / 8;
    if(byte >= this->ByteCount())
        this->value.resize(byte + 1);
    
    if(value)
        this->value[byte] |= (1 << (bit % 8));
    else
        this->value[byte] &= ~(1 << (bit % 8));
}

bool sosc::BigUInt::GetBit(uint64_t bit) const {
    size_t byte = bit / 8;
    if(byte >= this->ByteCount())
        return false;
    
    return (this->value[byte] & (1 << (bit % 8))) != 0;
}

sosc::BigUInt sosc::BigUInt::operator + (const BigUInt& rhs) const {
    size_t sum_range = std::max(this->ByteCount(), rhs.ByteCount());
    
    auto rhs_v = rhs.value;
    auto this_v = this->value;
    rhs_v.resize(sum_range, 0);
    this_v.resize(sum_range, 0);
    
    BigUInt sum;
    sum.value.clear();
    
    uint8_t carry = 0;
    for(size_t i = 0; i < sum_range; ++i) {
        uint16_t result = this_v[i] + rhs_v[i] + carry;
        carry = result >> 8;
        
        sum.value.push_back(result);
    }
    
    if(carry != 0)
        sum.value.push_back(carry);
    
    return sum;
}

sosc::BigUInt& sosc::BigUInt::operator += (const BigUInt& rhs) {
    this->Copy(*this + rhs);
    return *this;
}

sosc::BigUInt sosc::BigUInt::operator - (const BigUInt& rhs) const {
    if(*this < rhs)
        return BigUInt();
    
    size_t sub_range = std::max(this->ByteCount(), rhs.ByteCount());
    
    auto rhs_v = rhs.value;
    auto this_v = this->value;
    rhs_v.resize(sub_range, 0);
    this_v.resize(sub_range, 0);
    
    BigUInt sub;
    sub.value.clear();
    
    uint8_t carry = 0;
    for(size_t i = 0; i < sub_range; ++i) {
        uint8_t result = this_v[i] - rhs_v[i] - carry;
        carry = (rhs_v[i] + carry > this_v[i]) ? 1 : 0;
            
        sub.value.push_back(result);
    }
    
    return sub;
}

sosc::BigUInt& sosc::BigUInt::operator -= (const BigUInt& rhs) {
    this->Copy(*this - rhs);
    return *this;
}

sosc::BigUInt sosc::BigUInt::operator * (const BigUInt& rhs) const {
    if(this->IsZero() || rhs.IsZero())
        return BigUInt();
    if(this->IsOne())
        return rhs;
    if(rhs.IsOne())
        return *this;
    
    size_t prod_range = std::max(this->ByteCount(), rhs.ByteCount());
    
    auto rhs_v = rhs.value;
    auto this_v = this->value;
    rhs_v.resize(prod_range, 0);
    this_v.resize(prod_range, 0);
    
    BigUInt product;
    for(size_t i = 0; i < prod_range; ++i) {
        if(rhs_v[i] == 0)
            continue;
        
        for(size_t j = 0; j < prod_range; ++j) {
            if(this_v[j] == 0)
                continue;
            
            BigUInt result((uint16_t)((uint16_t)this_v[j] * rhs_v[i]));
            product += (result << (8 * (i + j)));
        }
    }
    
    return product;
}

sosc::BigUInt& sosc::BigUInt::operator *= (const BigUInt& rhs) {
    this->Copy(*this * rhs);
    return *this;
}

sosc::BigUInt sosc::BigUInt::operator / (const BigUInt& rhs) const {
    return BigUInt::DivideWithRemainder(*this, rhs).result;
}

sosc::BigUInt& sosc::BigUInt::operator /= (const BigUInt& rhs) {
    this->Copy(*this / rhs);
    return *this;
}

sosc::BigUInt sosc::BigUInt::operator % (const BigUInt& rhs) const {
    return BigUInt::DivideWithRemainder(*this, rhs).remainder;
}

bool sosc::BigUInt::operator == (const BigUInt& rhs) const {
    if(this->UsedByteCount() != rhs.UsedByteCount())
        return false;
    
    for(size_t i = 0; i < this->UsedByteCount(); ++i)
        if(this->value[i] != rhs.value[i])
            return false;
    
    return true;
}

bool sosc::BigUInt::operator != (const BigUInt& rhs) const {
    return !(*this == rhs);
}

bool sosc::BigUInt::operator > (const BigUInt& rhs) const {
    if(this->UsedByteCount() < rhs.UsedByteCount())
        return false;
    if(this->UsedByteCount() > rhs.UsedByteCount())
        return true;
    
    size_t msb = this->UsedByteCount() - 1;
    for(size_t i = msb;; --i) {
        if(this->value[i] != rhs.value[i])
            return this->value[i] > rhs.value[i];
        
        if(i == 0)
            return false;
    }
}

bool sosc::BigUInt::operator >= (const BigUInt& rhs) const {
    return (*this > rhs) || (*this == rhs);
}

bool sosc::BigUInt::operator < (const BigUInt& rhs) const {
    return !(*this > rhs) && *this != rhs;
}

bool sosc::BigUInt::operator <= (const BigUInt& rhs) const {
    return !(*this > rhs) || *this == rhs;
}

sosc::BigUInt sosc::BigUInt::operator >> (const uint64_t& rhs) const {
    size_t bytes = rhs / 8;
    uint8_t bits = rhs % 8;
    if(bytes >= this->ByteCount())
        return BigUInt();
    
    std::vector<uint8_t> this_v
        (this->value.begin() + bytes, this->value.end());
    std::vector<uint8_t> buffer(this_v.size(), 0);
    
    if(bits != 0) {
        uint8_t carry = 0, mask = 0;
        for(uint8_t i = 0; i < bits; ++i)
            mask |= (1 << i);
        
        for(size_t i = this_v.size() - 1;; --i) {
            buffer[i] = carry | (this_v[i] >> bits);
            carry = (buffer[i] & mask) << (8 - bits);
            
            if(i == 0)
                break;
        }
    }
    
    sosc::BigUInt shifted;
    shifted.value = bits == 0 ? this_v : buffer;
    return shifted;
}

sosc::BigUInt sosc::BigUInt::operator << (const uint64_t& rhs) const {
    size_t bytes = rhs / 8;
    uint8_t bits = rhs % 8;
    
    std::vector<uint8_t> this_v = this->value;
    for(size_t i = 0; i < bytes; ++i)
        this_v.insert(this_v.begin(), 0);
    std::vector<uint8_t> buffer(this_v.size(), 0);
    
    if(bits != 0) {
        uint8_t carry = 0, mask = 0;
        for(uint8_t i = 0; i < bits; ++i)
            mask |= (0x80 >> i);
        
        for(size_t i = bytes; i < this_v.size(); i++) {
            buffer[i] = carry | (this_v[i] << bits);
            carry = (this_v[i] & mask) >> (8 - bits);
        }
        
        if(carry != 0)
            buffer.push_back(carry);
    }
    
    sosc::BigUInt shifted;
    shifted.value = bits == 0 ? this_v : buffer;
    return shifted;
}

std::string sosc::BigUInt::ToString() const {
    std::stringstream stream;
    for(size_t i = this->ByteCount() - 1;; --i) {
        stream << std::setfill('0') 
               << std::setw(2) 
               << std::hex 
               << (int)this->value[i];
              
        if(i == 0)
            break;
    }
    
    return stream.str();
}
