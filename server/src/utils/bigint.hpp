#ifndef SOSC_UTIL_BIGINT_H
#define SOSC_UTIL_BIGINT_H

#include <vector>
#include <cstdint>

namespace sosc {
class BigInteger {
public:
    
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
