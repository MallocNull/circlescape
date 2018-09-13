#ifndef SOSC_CLIENT_COMMON_H
#define SOSC_CLIENT_COMMON_H

#include <string>

#ifdef SOSC_DEBUG
#define SOSC_RESOURCE_PATH (std::string("../resources/client/"))
#else
#define SOSC_RESOURCE_PATH (std::string("resources/"))
#endif

#define SOSC_RESC(X) (SOSC_RESOURCE_PATH + std::string(X))

#endif
