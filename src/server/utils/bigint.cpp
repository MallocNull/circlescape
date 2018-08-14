#include "bigint.hpp"

static bool is_hex_char(char c) {
    return ((c >= 'a') && (c <= 'f'))
        || ((c >= 'A') && (c <= 'F'))
        || ((c >= '0') && (c <= '9'));
}

static const uint32_t right_shift_masks[32] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 
    0x0000003f, 0x0000007f, 0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff, 
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff, 0x0001ffff, 
    0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff, 
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff, 
    0x3fffffff, 0x7fffffff
};

static const uint32_t left_shift_masks[32] = {
    0x00000000, 0x80000000, 0xc0000000, 0xe0000000, 0xf0000000, 0xf8000000, 
    0xfc000000, 0xfe000000, 0xff000000, 0xff800000, 0xffc00000, 0xffe00000, 
    0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000, 0xffff8000, 
    0xffffc000, 0xffffe000, 0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00, 
    0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0, 0xfffffff8, 
    0xfffffffc, 0xfffffffe
};

sosc::BigUInt::BigUInt() {
    this->value = { 0 };
}

sosc::BigUInt::BigUInt(std::string hex_str, uint64_t byte_count) {
    this->Parse(hex_str, byte_count);
}

bool sosc::BigUInt::Parse(std::string hex_str, uint64_t byte_count) {
    if(hex_str.length() == 0) 
        hex_str = "00000000";
    if(hex_str.length() % 8 != 0)
        for(int i = hex_str.length() % 8; i < 8; ++i)
            hex_str.insert(hex_str.begin(), '0');
    
    uint64_t word_count = WORD_CNT(byte_count);
    uint64_t str_word_count = hex_str.length() / 8;
    this->value = 
        std::vector<uint32_t>(std::max(word_count, str_word_count), 0);
        
    for(int i = 0; i < hex_str.length(); i += 8) {
        for(int j = 0; j < 8; ++j)
            if(!is_hex_char(hex_str[i + j]))
                return false;
        
        this->value[str_word_count - (i / 8) - 1] 
            = std::stoul(hex_str.substr(i, 8), 0, 16);
    }
    
    if(byte_count != 0)
        this->value.resize(word_count, 0);
        
    this->TrimLeadingZeroes();
    return true;
}

void sosc::BigUInt::Random(uint64_t byte_count) {
    this->value = std::vector<uint32_t>(WORD_CNT(byte_count), 0);
    std::string random_str = csprng::next_bytes(byte_count);
    for(int i = 0; i < this->value.size(); i++)
        for(int j = 0; j < 4; ++j)
            this->value[i] |= 
                (i * 4 + j < byte_count)
                    ? (uint8_t)random_str[i * 4 + j] << (8 * j)
                    : 0;
}

sosc::BigUInt sosc::BigUInt::GenerateRandom(uint64_t byte_count) {
    BigUInt num;
    num.Random(byte_count);
    return num;
}

void sosc::BigUInt::RandomPrime(uint64_t byte_count) {
    do {
        this->Random(byte_count);
        this->value[0] |= 0x01;
        this->value[this->value.size() - 1]
            |= (0x80 << (8 * BYTE_OFF(byte_count)));
    } while(!this->IsProbablePrime());
}

sosc::BigUInt sosc::BigUInt::GenerateRandomPrime(uint64_t byte_count) {
    BigUInt num;
    num.RandomPrime(byte_count);
    return num;
}

size_t sosc::BigUInt::UsedByteCount() const {
    return this->UsedWordCount() * 4;
}

size_t sosc::BigUInt::UsedWordCount() const {
    return this->WordCount();
}

bool sosc::BigUInt::IsZero() const {
    return this->value.size() == 1 && this->value[0] == 0;
}

bool sosc::BigUInt::IsOne() const {
    return this->value.size() == 1 && this->value[0] == 1;
}

bool sosc::BigUInt::IsEven() const {
    return !this->GetBit(0);
}

bool sosc::BigUInt::IsProbablePrime(uint16_t rounds) const {
    if(this->IsOne())
        return false;
    
    if    (this->IsEven() 
        || this->IsDivisibleBy(BigUInt(3u))
        || this->IsDivisibleBy(BigUInt(5u)))
    {
        return false;
    }
    
    for(uint16_t i = 0; i < rounds; ++i) {
        BigUInt rnd = BigUInt::GenerateRandom(this->WordCount());
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
    
    size_t upper_bound = num.UsedByteCount() * 8 - 1;
    BigUInt quotient, remainder;
    for(size_t i = upper_bound;; --i) {
        remainder = remainder << 1;
        remainder.SetBit(0, num.GetBit(i));
                  
        if(remainder >= denom) {
            remainder -= denom;
            quotient.SetBit(i, true);
        }
        
        if(i == 0)
            break;
    }
    
    if(remainder.value[remainder.value.size() - 1] == 0)
        remainder.value.erase(remainder.value.end() - 1);
    return division_t(quotient, remainder);
}

sosc::BigUInt sosc::BigUInt::ModPow
    (const BigUInt& base, const BigUInt& exp, const BigUInt& mod)
{
    BigUInt accum("1");
    BigUInt x = exp;
    BigUInt bpow = base;
    
    uint64_t upper_bound = exp.UsedByteCount() * 8;
    for(uint64_t i = 0; i < upper_bound; ++i) {
        if(x.GetBit(i))
            accum = (accum * bpow) % mod;
        
        bpow = (bpow * bpow) % mod;
    }
    
    return accum;
}

void sosc::BigUInt::SetBit(uint64_t bit, bool value) {
    size_t word = bit / 32;
    if(word >= this->WordCount())
        this->value.resize(word + 1);
    
    if(value)
        this->value[word] |= (1ul << (bit % 32));
    else
        this->value[word] &= ~(1ul << (bit % 32));
}

bool sosc::BigUInt::GetBit(uint64_t bit) const {
    size_t word = bit / 32;
    if(word >= this->WordCount())
        return false;
    
    return (this->value[word] & (1ul << (bit % 32))) != 0;
}

sosc::BigUInt sosc::BigUInt::operator + (const BigUInt& rhs) const {
    size_t sum_range = std::max(this->WordCount(), rhs.WordCount());
    
    auto rhs_v = rhs.value;
    auto this_v = this->value;
    rhs_v.resize(sum_range, 0);
    this_v.resize(sum_range, 0);
    
    BigUInt sum;
    sum.value.clear();
    
    uint32_t carry = 0;
    for(size_t i = 0; i < sum_range; ++i) {
        uint64_t result = (uint64_t)this_v[i] + rhs_v[i] + carry;
        carry = result >> 32;
        
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
    
    size_t sub_range = std::max(this->WordCount(), rhs.WordCount());
    
    auto rhs_v = rhs.value;
    auto this_v = this->value;
    rhs_v.resize(sub_range, 0);
    this_v.resize(sub_range, 0);
    
    BigUInt sub;
    sub.value.clear();
    
    uint64_t carry = 0;
    for(size_t i = 0; i < sub_range; ++i) {
        uint32_t result = this_v[i] - rhs_v[i] - carry;
        carry = (rhs_v[i] + carry > this_v[i]) ? 1 : 0;
            
        sub.value.push_back(result);
    }
    
    sub.TrimLeadingZeroes();
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
    
    size_t prod_range = std::max(this->WordCount(), rhs.WordCount());
    
    /*auto rhs_v = rhs.value;
    auto this_v = this->value;
    rhs_v.resize(prod_range, 0);
    this_v.resize(prod_range, 0);*/
    
    BigUInt product;
    /*for(size_t i = 0; i < prod_range; ++i) {
        if(rhs_v[i] == 0)
            continue;
        
        for(size_t j = 0; j < prod_range; ++j) {
            if(this_v[j] == 0)
                continue;
            
            BigUInt result((uint64_t)((uint64_t)this_v[j] * rhs_v[i]));
            product += (result << (32 * (i + j)));
        }
    }*/
    
    BigUInt this_cpy = *this;
    uint64_t upper_bound = rhs.UsedByteCount() * 8;
    for(uint64_t i = 0; i < upper_bound; ++i) { 
        if(rhs.GetBit(i))
            product += this_cpy;
        
        this_cpy = this_cpy << 1;
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
    if(this->UsedWordCount() != rhs.UsedWordCount())
        return false;
    
    for(size_t i = 0; i < this->UsedWordCount(); ++i)
        if(this->value[i] != rhs.value[i])
            return false;
    
    return true;
}

bool sosc::BigUInt::operator != (const BigUInt& rhs) const {
    return !(*this == rhs);
}

bool sosc::BigUInt::operator > (const BigUInt& rhs) const {
    if(this->UsedWordCount() < rhs.UsedWordCount())
        return false;
    if(this->UsedWordCount() > rhs.UsedWordCount())
        return true;
    
    size_t msb = this->UsedWordCount() - 1;
    for(size_t i = msb;; --i) {
        if(this->value[i] != rhs.value[i])
            return this->value[i] > rhs.value[i];
        
        if(i == 0)
            return false;
    }
}

bool sosc::BigUInt::operator >= (const BigUInt& rhs) const {
    return *this > rhs || *this == rhs;
}

bool sosc::BigUInt::operator < (const BigUInt& rhs) const {
    return !(*this > rhs) && *this != rhs;
}

bool sosc::BigUInt::operator <= (const BigUInt& rhs) const {
    return !(*this > rhs) || *this == rhs;
}

sosc::BigUInt sosc::BigUInt::operator >> (const uint64_t& rhs) const {
    size_t words = rhs / 32;
    uint32_t bits = rhs % 32;
    if(words >= this->WordCount())
        return BigUInt();
    
    std::vector<uint32_t> this_v
        (this->value.begin() + words, this->value.end());
    std::vector<uint32_t> buffer(this_v.size(), 0);
    
    if(bits != 0) {
        uint32_t carry = 0, mask = right_shift_masks[bits];
        
        for(size_t i = this_v.size() - 1;; --i) {
            buffer[i] = carry | (this_v[i] >> bits);
            carry = (this_v[i] & mask) << (32 - bits);
            
            if(i == 0)
                break;
        }
        
        if(buffer[buffer.size() - 1] == 0)
            buffer.erase(buffer.end() - 1);
    }
    
    sosc::BigUInt shifted;
    shifted.value = bits == 0 ? this_v : buffer;
    return shifted;
}

sosc::BigUInt sosc::BigUInt::operator << (const uint64_t& rhs) const {
    size_t words = rhs / 32;
    uint32_t bits = rhs % 32;
    
    std::vector<uint32_t> this_v = this->value;
    for(size_t i = 0; i < words; ++i)
        this_v.insert(this_v.begin(), 0);
    std::vector<uint32_t> buffer(this_v.size(), 0);
    
    if(bits != 0) {
        uint32_t carry = 0, mask = left_shift_masks[bits];
        
        for(size_t i = words; i < this_v.size(); i++) {
            buffer[i] = carry | (this_v[i] << bits);
            carry = (this_v[i] & mask) >> (32 - bits);
        }
        
        if(carry != 0)
            buffer.push_back(carry);
    }
    
    sosc::BigUInt shifted;
    shifted.value = bits == 0 ? this_v : buffer;
    return shifted;
}

std::string sosc::BigUInt::ToRawString(uint64_t byte_count) const {
    uint64_t used_bytes = this->UsedByteCount();
    byte_count = byte_count == 0 ? used_bytes : byte_count;
    std::string raw(byte_count, 0);
    
    for(uint64_t i = 0; i < std::min(byte_count, used_bytes); ++i)
        raw[i] = (this->value[i / 4] >> (8 * (i % 4))) & 0xFF;
    
    return raw;
}

std::string sosc::BigUInt::ToString() const {
    std::stringstream stream;
    for(size_t i = this->WordCount() - 1;; --i) {
        stream << std::setfill('0') 
               << std::setw(8) 
               << std::hex 
               << this->value[i];
              
        if(i == 0)
            break;
    }
    
    return stream.str();
}
