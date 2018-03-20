#ifndef SOSC_CRYPTO_BASE64
#define SOSC_CRYPTO_BASE64

#include <vector>
#include <string>
#include <iostream>

namespace sosc {
namespace cgc {
std::string base64_encode(const std::string& data, bool unix = false);
std::string base64_encode(const void* raw, size_t length, bool unix = false);

std::string base64_decode(const std::string& data, bool unix = false);
}}

#endif
