#ifndef SOSC_UI_TEXT_H
#define SOSC_UI_TEXT_H

#include <SDL.h>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <SDL_image.h>

#include <string>
#include <fstream>
#include "utils/net.hpp"

namespace sosc {
namespace ui {
class Font {
public:
    struct glyph_t {
        double width;
        glm::vec2
            top_left,
            top_right,
            bottom_left,
            bottom_right;
    };

    Font() : loaded(false) {}
    bool Load(const std::string& bitmapPath, const std::string& dataPath);

    inline const glyph_t& operator[] (char c) {
        return this->glyphs[(uint8_t)c];
    }



    void Unload();
private:
    bool loaded;
    GLuint texture;
    SDL_Surface* image;
    uint32_t
        width, height,
        cell_width, cell_height;
    glyph_t glyphs[256];
};

class Text {
public:
    Text(Font* font) : loaded(false) {}


private:
    Font* font;
    GLuint vao, vbo[2];
    bool loaded;
};
}}

#endif
