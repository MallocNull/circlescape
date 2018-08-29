#ifndef SOSC_UI_FONT_H
#define SOSC_UI_FONT_H

#include <SDL.h>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <SDL_image.h>

#include <string>
#include <fstream>
#include "utils/net.hpp"
#include "shaders/_shader.hpp"

namespace sosc {
namespace ui {
class Font;
bool font_init_subsystem(SDL_Window* window);
void font_set_default(sosc::ui::Font* font);
void font_window_changed(SDL_Window* window);
void font_deinit_subsystem();

class FontException : public std::exception {
public:
    explicit FontException
        (const std::string& bitmapPath, const std::string& dataPath)
    {
        std::stringstream ss;
        ss << "Could not load bitmap file '" << bitmapPath << "' "
           << "and/or data file '" << dataPath << "'.";

        this->errorInfo = ss.str();
    }

    const std::string& what() noexcept {
        return this->errorInfo;
    }
private:
    std::string errorInfo;
};

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
    Font(const std::string& bitmapPath, const std::string& dataPath);
    bool Load(const std::string& bitmapPath, const std::string& dataPath);

    inline const glyph_t& operator[] (char c) const {
        return this->glyphs[(uint8_t)c];
    }

    void Unload();
private:
    void BindBitmap();
    void UnbindBitmap();

    bool loaded;
    GLuint texture;
    SDL_Surface* image;
    uint32_t
        width, height,
        cell_width, cell_height;
    glyph_t glyphs[256];

    friend class Text;
};

class Text {
public:
    Text();
    Text(Font* font, uint32_t size);
    Text(uint32_t size, const std::string& text,
         uint32_t x, uint32_t y, uint32_t w = 0);
    Text(Font* font, uint32_t size, const std::string& text,
         uint32_t x, uint32_t y, uint32_t w = 0);

    void Set(uint32_t size, const std::string& text,
             uint32_t x, uint32_t y, uint32_t w = 0);
    void SetFont(Font* font, uint32_t size = 0);
    void SetFontSize(uint32_t size);
    void SetText(const std::string& text);
    void SetPosition(uint32_t x, uint32_t y);
    void SetWrapWidth(uint32_t w);

    void Render();

    void Destroy();
private:
    void Redraw();

    Font* font;
    uint32_t font_size,
             wrap_width;
    std::string text;
    glm::mat4 trans_matrix;
    GLsizei vertex_count;
    GLuint vao, vbos[2];
};
}}

#endif