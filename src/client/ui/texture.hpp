#ifndef SOSC_UI_TEXTURE_H
#define SOSC_UI_TEXTURE_H

#include <SDL.h>
#include <GL/glew.h>
#include "utils/time.hpp"

#include <iterator>
#include <forward_list>
#include <chrono>

namespace sosc {
namespace ui {
class Texture {
public:

private:
    sosc::time last_render;
    std::forward_list<GLuint> texture_ids;
    std::forward_list<GLuint>::iterator texture_id_iter;
};
}}

#endif
