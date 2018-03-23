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

#define BYTE_OFF(X) ((X - 1) % 4)
#define WORD_CNT(X) (X == 0 ? 0 : ((X - 1) / 4 + 1))

namespace sosc {
struct division_t;
    
class BigUInt {
public:
    BigUInt();
    
    BigUInt(uint8_t value)  { this->Initialize(value); }
    BigUInt(uint16_t value) { this->Initialize(value); }
    BigUInt(uint32_t value) { this->Initialize(value); }
    BigUInt(uint64_t value) { this->Initialize(value); }
    
    BigUInt(std::string hex_str, uint64_t byte_count = 0);
    bool Parse(std::string hex_str, uint64_t byte_count = 0);
    inline void Resize(uint64_t byte_count) {
        this->value.resize(WORD_CNT(byte_count), 0);
    }
    
    void Random(uint64_t byte_count);
    void RandomPrime(uint64_t byte_count);
    static BigUInt GenerateRandom(uint64_t byte_count);
    static BigUInt GenerateRandomPrime(uint64_t byte_count);
    
    size_t UsedByteCount() const;
    size_t UsedWordCount() const;
    inline size_t WordCount() const {
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
        this->value.clear();
        for(int i = 0; i < WORD_CNT(sizeof(T)); ++i) {
            this->value.push_back(value & 0xFFFFFFFF);
            value >>= 32;
        }
    }
    
    inline void Copy(const BigUInt& from) {
        this->value = from.value;
    }
    
    std::vector<uint32_t> value;
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
