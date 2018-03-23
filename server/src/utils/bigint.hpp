#ifndef SOSC_UTIL_BIGINT_H
#define SOSC_UTIL_BIGINT_H

#include <iostream>

#include <cassert>
#include <tuple>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "csprng.hpp"

namespace sosc {
struct division_t;
    
class BigUInt {
public:
    BigUInt();
    
    BigUInt(uint8_t value)  { this->Initialize(value); }
    BigUInt(uint16_t value) { this->Initialize(value); }
    BigUInt(uint32_t value) { this->Initialize(value); }
    BigUInt(uint64_t value) { this->Initialize(value); }
    
    BigUInt(std::string hex_str, int byte_count = -1);
    bool Parse(std::string hex_str, int byte_count = -1);
    inline void Resize(uint64_t byte_count) {
        this->value.resize(byte_count, 0);
    }
    
    void Random(int byte_count);
    void RandomPrime(int byte_count);
    static BigUInt GenerateRandom(int byte_count);
    static BigUInt GenerateRandomPrime(int byte_count);
    
    size_t UsedByteCount() const;
    inline size_t ByteCount() const {
        return value.size();
    }
    
    bool IsZero() const;
    bool IsOne() const;
    bool IsEven() const;
    bool IsProbablePrime(uint16_t rounds = 5) const;
    bool IsDivisibleBy(const BigUInt& value) const;
    
    static division_t DivideWithRemainder
        (const BigUInt& num, const BigUInt& denom);
        
    static BigUInt ModPow
        (const BigUInt& base, const BigUInt& exp, const BigUInt& mod);
    
    void SetBit(uint64_t bit, bool value);
    bool GetBit(uint64_t bit) const;
        
    BigUInt  operator +  (const BigUInt& rhs) const;
    BigUInt& operator += (const BigUInt& rhs);
    
    BigUInt  operator -  (const BigUInt& rhs) const;
    BigUInt& operator -= (const BigUInt& rhs);
    
    BigUInt  operator *  (const BigUInt& rhs) const;
    BigUInt& operator *= (const BigUInt& rhs);
    
    BigUInt  operator /  (const BigUInt& rhs) const;
    BigUInt& operator /= (const BigUInt& rhs);
    
    BigUInt  operator %  (const BigUInt& rhs) const;
    
    bool operator == (const BigUInt& rhs) const;
    bool operator != (const BigUInt& rhs) const;
    bool operator >  (const BigUInt& rhs) const;
    bool operator >= (const BigUInt& rhs) const;
    bool operator <  (const BigUInt& rhs) const;
    bool operator <= (const BigUInt& rhs) const;
    
    BigUInt operator >> (const uint64_t& rhs) const;
    BigUInt operator << (const uint64_t& rhs) const;
    
    std::string ToString() const;
    inline operator std::string () const {
        return this->ToString();
    }
private:
    template<typename T>
    void Initialize(T value) {
        for(int i = 0; i < sizeof(T); ++i) {
            this->value.push_back(value & 0xFF);
            value >>= 8;
        }
    }
    
    inline void Copy(const BigUInt& from) {
        this->value = from.value;
    }
    
    std::vector<uint8_t> value;
};

struct division_t {
public:
    BigUInt result;
    BigUInt remainder;
private:
    division_t(const BigUInt& result, const BigUInt& remainder) {
        this->result = result;
        this->remainder = remainder;
    }
    
    friend class BigUInt;
};
}

/*
      FFFFFF
        FFFF x
      ========
        FE01
      FE0100
    FE010000 +
    ==========
    FEFFFF01
      FE0100
    FE010000
  FE01000000 +
  ============
  FFFEFF0001
*/

#endif
