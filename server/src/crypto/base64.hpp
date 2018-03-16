#ifndef SOSC_CRYPTO_BASE64
#define SOSC_CRYPTO_BASE64

#include <vector>
#include <string>
#include <iostream>

namespace sosc {
namespace cgc {
std::string base64_encode(const std::string& data);
std::string base64_encode(const void* raw, size_t length);

std::string base64_decode(const std::string& data);
}}

#endif
