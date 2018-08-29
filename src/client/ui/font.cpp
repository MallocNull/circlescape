#include "font.hpp"

#define LILEND_UNPACK(X,N) \
     (((uint32_t)(X)[N])              | \
    ((((uint32_t)(X)[(N)+1])) << 8u)  | \
    ((((uint32_t)(X)[(N)+2])) << 16u) | \
    ((((uint32_t)(X)[(N)+3])) << 24u))

// FONT SHADER CLASS //

namespace sosc {
namespace ui {
static class FontShader : public sosc::shdr::Shader {
public:
    enum Uniforms {
        ORTHO_MATRIX = 0,
        TRANSLATION_MATRIX,
        FONT_BITMAP,
        FONT_COLOR
    };

    void UpdateWindow(SDL_Window *window) {
        this->Start();

        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        glm::mat4 orthoMatrix = glm::ortho(0, width, height, 0);
        glUniformMatrix4fv(
            (*this)[ORTHO_MATRIX], 1, GL_FALSE, glm::value_ptr(orthoMatrix)
        );

        this->Stop();
    }

protected:
    void PrepareLoad() override {
        this->AttachSource(
            SOSC_RESC("shaders/font/font.vert"), GL_VERTEX_SHADER
        );
        this->AttachSource(
            SOSC_RESC("shaders/font/font.frag"), GL_FRAGMENT_SHADER
        );

        this->LinkProgram();
        this->LoadUniforms({
            "orthoMatrix",
            "transMatrix"
            "fontBitmap",
            "fontColor"
        });
    }
};
}}

// STATE STRUCT //

static struct {
    sosc::ui::FontShader shader;
    sosc::ui::Font* default_font;
} _font_ctx;

// SUBSYSTEM FUNCS //

bool sosc::ui::font_init_subsystem(SDL_Window* window) {
    _font_ctx.shader.Load();
    _font_ctx.shader.UpdateWindow(window);
    _font_ctx.default_font = nullptr;
}

void sosc::ui::font_set_default(Font *font) {
    _font_ctx.default_font = font;
}

void sosc::ui::font_window_changed(SDL_Window* window) {
    _font_ctx.shader.UpdateWindow(window);
}

void sosc::ui::font_deinit_subsystem() {
    _font_ctx.shader.Unload();
}

// FONT CLASS //

sosc::ui::Font::Font
    (const std::string& bitmapPath, const std::string& dataPath)
{
    if(!this->Load(bitmapPath, dataPath))
        throw new FontException(bitmapPath, dataPath);
}

bool sosc::ui::Font::Load
    (const std::string& filePath, const std::string& dataPath)
{
    if(this->loaded)
        this->Unload();

    SDL_RWops* rwop = SDL_RWFromFile(filePath.c_str(), "rb");
    this->image = SDL_LoadBMP_RW(rwop, 1);
    if(!this->image)
        return false;

    char buffer[0x111];
    std::ifstream dataFile(dataPath, std::ios::in | std::ios::binary);
    if(!dataFile.is_open())
        return false;
    dataFile.read(buffer, 0x111);
    dataFile.close();
    std::string data(buffer, 0x111);
    if(buffer[0x10] != 0)
        return false;

    this->width = (uint32_t)this->image->w;
    this->height = (uint32_t)this->image->h;

    this->cell_width = LILEND_UNPACK(buffer, 0x08);
    this->cell_width = LILEND_UNPACK(buffer, 0x0C);

    for(int i = 0; i < 256; ++i) {
        auto glyph = &this->glyphs[i];
        auto width = (uint8_t)buffer[0x11 + i];

        glyph->width = (double)width / (double)this->cell_width;

        int x = (this->cell_width * i) % this->width;
        int y = ((this->cell_width * i) / this->width) * this->cell_height;

        glyph->top_left = glm::vec2(
            (double)x / (double)this->width,
            1 - (double)y / (double)this->height
        );
        glyph->top_right = glm::vec2(
            (double)(x + width) / (double)this->width,
            1 - (double)y / (double)this->height
        );
        glyph->bottom_left = glm::vec2(
            (double)x / (double)this->width,
            1 - (double)(y + this->cell_height) / (double)this->height
        );
        glyph->bottom_right = glm::vec2(
            (double)(x + width) / (double)this->width,
            1 - (double)(y + this->cell_height) / (double)this->height
        );
    }

    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        this->width, this->height, 0,
        (this->image->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB),
        GL_UNSIGNED_BYTE, this->image->pixels
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    this->loaded = true;
}

void sosc::ui::Font::BindBitmap() {
    if(!this->loaded)
        return;

    glBindTexture(GL_TEXTURE_2D, this->texture);
}

void sosc::ui::Font::UnbindBitmap() {
    if(!this->loaded)
        return;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void sosc::ui::Font::Unload() {
    glDeleteTextures(1, &this->texture);
    SDL_FreeSurface(this->image);

    this->loaded = false;
}

// TEXT CLASS //

sosc::ui::Text::Text() {
    this->font = _font_ctx.default_font;
    this->font_size = 0;

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(2, this->vbos);
    glBindVertexArray(0);
}

sosc::ui::Text::Text(sosc::ui::Font *font, uint32_t size) : Text() {
    this->font = font;
    this->font_size = size;
}

sosc::ui::Text::Text(uint32_t size, const std::string &text,
    uint32_t x, uint32_t y, uint32_t w) : Text()
{
    this->Set(size, text, x, y, w);
}

sosc::ui::Text::Text(sosc::ui::Font *font, uint32_t size,
    const std::string &text, uint32_t x, uint32_t y, uint32_t w) : Text()
{
    this->font = font;
    this->Set(size, text, x, y, w);
}

void sosc::ui::Text::Set
    (uint32_t size, const std::string &text, uint32_t x, uint32_t y, uint32_t w)
{
    this->font_size = size;
    this->text = text;
    if(w != 0)
        this->wrap_width = w;
    this->trans_matrix = glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f));
    this->Redraw();
}

void sosc::ui::Text::SetFont(Font *font, uint32_t size) {
    this->font = font;
    if(size != 0)
        this->font_size = size;
    this->Redraw();
}

void sosc::ui::Text::SetFontSize(uint32_t size) {
    this->font_size = size;
    this->Redraw();
}

void sosc::ui::Text::SetText(const std::string &text) {
    this->text = text;
    this->Redraw();
}

void sosc::ui::Text::SetPosition(uint32_t x, uint32_t y) {
    this->trans_matrix = glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f));
}

void sosc::ui::Text::SetWrapWidth(uint32_t w) {
    this->wrap_width = w;
    this->Redraw();
}

void sosc::ui::Text::Render() {
    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
    glBindVertexArray(0);
}

void sosc::ui::Text::Destroy() {
    glDeleteBuffers(2, this->vbos);
    glDeleteVertexArrays(1, &this->vao);
}

void sosc::ui::Text::Redraw() {
    this->vertex_count = ;
    uint32_t line_width = 0, top_x = 0, top_y = 0;
    for(const auto c : this->text) {
        auto glyph = (*this->font)[c];
        uint32_t width = (uint32_t)(this->font_size * glyph.width),
                height = this->font_size;


    }
}