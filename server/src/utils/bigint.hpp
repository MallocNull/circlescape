#ifndef SOSC_UTIL_BIGINT_H
#define SOSC_UTIL_BIGINT_H

#include <tuple>
#include <string>
#include <vector>
#include <cstdint>

namespace sosc {
class BigInteger {
public:
    BigInteger();
    BigInteger(std::string hex_str, int byte_count = -1);
    
    inline size_t ByteCount() {
        return value.size();
    }
    
    static std::tuple<BigInteger, BigInteger> DivideWithRemainder
        (const BigInteger& num, const BigInteger& denom);
        
    static BigInteger ModPow
        (const BigInteger& base, const BigInteger& exp, const BigInteger& mod);
    
    
        
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
