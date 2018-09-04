#include "font.hpp"

#define LILEND_UNPACK(X,N) \
     (((uint32_t)(X)[N])              | \
    ((((uint32_t)(X)[(N)+1])) << 8u)  | \
    ((((uint32_t)(X)[(N)+2])) << 16u) | \
    ((((uint32_t)(X)[(N)+3])) << 24u))

namespace sosc {
namespace ui {
class FontShader;
}}

// STATE STRUCT //

static struct {
    sosc::ui::FontShader* shader;
    sosc::ui::Font* default_font;
} _font_ctx;

// FONT SHADER CLASS //

namespace sosc {
namespace ui {
class FontShader : public sosc::shdr::Shader {
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
        glm::mat4 orthoMatrix =
            glm::ortho(0.f, (float)width, (float)height, 0.f);
        glUniformMatrix4fv(
            (*this)[ORTHO_MATRIX], 1, GL_FALSE,
            glm::value_ptr(orthoMatrix)
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
            "transMatrix",
            "fontBitmap",
            "fontColor"
        });
    }
};
}}

// SUBSYSTEM FUNCS //

void sosc::ui::font_init_subsystem(SDL_Window* window) {
    _font_ctx.shader = new FontShader();
    _font_ctx.shader->Load();
    _font_ctx.shader->UpdateWindow(window);
    _font_ctx.default_font = nullptr;
}

void sosc::ui::font_set_default(Font *font) {
    _font_ctx.default_font = font;
}

void sosc::ui::font_window_changed(SDL_Window* window) {
    _font_ctx.shader->UpdateWindow(window);
}

void sosc::ui::font_deinit_subsystem() {
    _font_ctx.shader->Unload();
    delete _font_ctx.shader;
}

// FONT CLASS //

sosc::ui::Font::Font
    (const std::string& bitmapPath, const std::string& dataPath,
     bool useNearest)
{
    this->loaded = false;
    if(!this->Load(bitmapPath, dataPath))
        throw FontException(bitmapPath, dataPath);
}

bool sosc::ui::Font::Load
    (const std::string& bitmapPath, const std::string& dataPath,
     bool useNearest)
{
    if(this->loaded)
        this->Unload();

    SDL_RWops* rwop = SDL_RWFromFile(bitmapPath.c_str(), "rb");
    SDL_Surface* image = SDL_LoadBMP_RW(rwop, 1);
    if(!image)
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

    this->width = (uint32_t)image->w;
    this->height = (uint32_t)image->h;

    this->cell_width = LILEND_UNPACK(buffer, 0x08);
    this->cell_height = LILEND_UNPACK(buffer, 0x0C);

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
        GL_TEXTURE_2D, 0, 3,
        this->width, this->height, 0,
        GL_BGR,
        GL_UNSIGNED_BYTE, image->pixels
    );

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        useNearest ? GL_NEAREST : GL_LINEAR
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        useNearest ? GL_NEAREST : GL_LINEAR
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(image);
    this->loaded = true;
    return true;
}

void sosc::ui::Font::BindBitmap() const {
    if(!this->loaded)
        return;

    glBindTexture(GL_TEXTURE_2D, this->texture);
}

void sosc::ui::Font::UnbindBitmap() const {
    if(!this->loaded)
        return;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void sosc::ui::Font::Unload() {
    glDeleteTextures(1, &this->texture);
    this->loaded = false;
}

// TEXT CLASS //

sosc::ui::Text::Text() {
    this->font = _font_ctx.default_font;
    this->font_size = 0;
    this->vertices = nullptr;
    this->tex_coords = nullptr;

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(2, this->vbos);
}

sosc::ui::Text::Text
    (sosc::ui::Font *font, uint32_t size, const glm::vec4& color) : Text()
{
    this->font = font;
    this->font_size = size;
    this->font_color = color;
}

sosc::ui::Text::Text(uint32_t size, const glm::vec4& color,
    const std::string &text, uint32_t x, uint32_t y, uint32_t w) : Text()
{
    this->Set(size, color, text, x, y, w);
}

sosc::ui::Text::Text
    (sosc::ui::Font *font, uint32_t size, const glm::vec4& color,
    const std::string &text, uint32_t x, uint32_t y, uint32_t w) : Text()
{
    this->font = font;
    this->Set(size, color, text, x, y, w);
}

void sosc::ui::Text::Set(uint32_t size, const glm::vec4& color,
    const std::string &text, uint32_t x, uint32_t y, uint32_t w)
{
    this->font_size = size;
    this->font_color = color;
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

void sosc::ui::Text::SetFontColor(const glm::vec4 &color) {
    this->font_color = color;
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

uint32_t sosc::ui::Text::GetHeight() const {
    return this->GetLineCount() * this->font_size;
}

uint32_t sosc::ui::Text::GetLineCount() const {
    if(this->wrap_width == 0)
        return 1;
    else {
        uint32_t count = 1, topleft_x = 0;
        for(const auto c : this->text) {
            auto width = (uint32_t)((*this->font)[c].width * this->font_size);

            if(topleft_x + width > this->wrap_width) {
                ++count;
                topleft_x = 0;
            }

            topleft_x += width;
        }
        return count;
    }
}

void sosc::ui::Text::Render() {
    auto shdr = _font_ctx.shader;

    shdr->Start();
    glUniformMatrix4fv(
        (*shdr)[shdr->TRANSLATION_MATRIX],
        1, GL_FALSE,
        glm::value_ptr(this->trans_matrix)
    );
    glUniform4f(
        (*shdr)[shdr->FONT_COLOR],
        this->font_color.r, this->font_color.g,
        this->font_color.b, this->font_color.a
    );

    glActiveTexture(GL_TEXTURE0);
    this->font->BindBitmap();

    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
    glBindVertexArray(0);

    this->font->UnbindBitmap();
    shdr->Stop();
}

void sosc::ui::Text::Destroy() {
    glDeleteBuffers(2, this->vbos);
    glDeleteVertexArrays(1, &this->vao);

    delete[] this->vertices;
    delete[] this->tex_coords;
}

void sosc::ui::Text::Redraw() {
    this->vertex_count = (GLuint)(6 * this->text.length());

    delete[] this->vertices;
    this->vertices = new float[this->vertex_count * 2];

    delete[]this->tex_coords;
    this->tex_coords = new float[this->vertex_count * 2];

    uint32_t top_x = 0, top_y = 0;
    for(int i = 0; i < this->text.length(); ++i) {
        auto glyph = (*this->font)[this->text[i]];
        uint32_t width = (uint32_t)(this->font_size * glyph.width),
                height = this->font_size;

        if(top_x + width > this->wrap_width && this->wrap_width != 0) {
            top_x = 0;
            top_y += height;
        }

        /// TRIANGLE 1 ///
        // TOP LEFT
        this->vertices[i*12] = top_x;
        this->vertices[i*12 + 1] = top_y;
        this->tex_coords[i*12] = glyph.top_left.x;
        this->tex_coords[i*12 + 1] = glyph.top_left.y;
        // TOP RIGHT
        this->vertices[i*12 + 2] = top_x + width;
        this->vertices[i*12 + 3] = top_y;
        this->tex_coords[i*12 + 2] = glyph.top_right.x;
        this->tex_coords[i*12 + 3] = glyph.top_right.y;
        // BOTTOM LEFT
        this->vertices[i*12 + 4] = top_x;
        this->vertices[i*12 + 5] = top_y + height;
        this->tex_coords[i*12 + 4] = glyph.bottom_left.x;
        this->tex_coords[i*12 + 5] = glyph.bottom_left.y;

        /// TRIANGLE 2 ///
        // BOTTOM LEFT
        this->vertices[i*12 + 6] = top_x;
        this->vertices[i*12 + 7] = top_y + height;
        this->tex_coords[i*12 + 6] = glyph.bottom_left.x;
        this->tex_coords[i*12 + 7] = glyph.bottom_left.y;
        // TOP RIGHT
        this->vertices[i*12 + 8] = top_x + width;
        this->vertices[i*12 + 9] = top_y;
        this->tex_coords[i*12 + 8] = glyph.top_right.x;
        this->tex_coords[i*12 + 9] = glyph.top_right.y;
        // BOTTOM RIGHT
        this->vertices[i*12 + 10] = top_x + width;
        this->vertices[i*12 + 11] = top_y + height;
        this->tex_coords[i*12 + 10] = glyph.bottom_right.x;
        this->tex_coords[i*12 + 11] = glyph.bottom_right.y;

        top_x += width;
    }

    glBindVertexArray(this->vao);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbos[0]);
        glBufferData(
            GL_ARRAY_BUFFER,
            this->vertex_count * 2 * sizeof(float),
            this->vertices,
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbos[1]);
        glBufferData(
            GL_ARRAY_BUFFER,
            this->vertex_count * 2 * sizeof(float),
            this->tex_coords,
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}