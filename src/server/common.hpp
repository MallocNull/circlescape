#ifndef SOSC_SERVER_COMMON_H
#define SOSC_SERVER_COMMON_H

#include "utils/ini.hpp"
#include <string>

#ifdef SOSC_DEBUG
#define SOSC_RESOURCE_PATH (std::string("../resources/server/"))
#else
#define SOSC_RESOURCE_PATH (std::string("resources/"))
#endif

#define SOSC_RESC(X) (SOSC_RESOURCE_PATH + std::string(X))

namespace sosc {
struct {
    ini::File* config = nullptr;
} _global_ctx;
}

#endif
