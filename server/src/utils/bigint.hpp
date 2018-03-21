#ifndef SOSC_UTIL_BIGINT_H
#define SOSC_UTIL_BIGINT_H

#include <tuple>
#include <string>
#include <vector>
#include <cstdint>
#include "csprng.hpp"

namespace sosc {
class BigUInt {
public:
    BigUInt();
    BigUInt(std::string hex_str, int byte_count = -1);
    bool Parse(std::string hex_str, int byte_count = -1);
    void Random(int byte_count);
    void RandomPrime(int byte_count);
    
    inline size_t ByteCount() const {
        return value.size();
    }
    
    bool IsZero() const;
    bool IsOne() const;
    bool IsEven() const;
    bool IsProbablePrime() const;
    
    static std::tuple<BigUInt, BigUInt> DivideWithRemainder
        (const BigUInt& num, const BigUInt& denom);
        
    static BigUInt ModPow
        (const BigUInt& base, const BigUInt& exp, const BigUInt& mod);
    
    void SetBit(uint64_t bit, bool value);
    bool GetBit(uint64_t bit) const;
        
    BigUInt& operator +  (const BigUInt& rhs) const;
    BigUInt& operator += (const BigUInt& rhs);
    
    BigUInt& operator -  (const BigUInt& rhs) const;
    BigUInt& operator -= (const BigUInt& rhs);
    
    BigUInt& operator *  (const BigUInt& rhs) const;
    BigUInt& operator *= (const BigUInt& rhs);
    
    BigUInt& operator /  (const BigUInt& rhs) const;
    BigUInt& operator /= (const BigUInt& rhs);
    
    BigUInt& operator %  (const BigUInt& rhs) const;
    
    bool operator == (const BigUInt& rhs) const;
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
    std::vector<uint8_t> value;
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
