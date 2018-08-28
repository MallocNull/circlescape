#include "text.hpp"

// FONT CLASS //

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
    this->cell_width = sosc::net::ntohv<uint32_t>(data, 0x08);
    this->cell_height = sosc::net::ntohv<uint32_t>(data, 0x0C);

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

void sosc::ui::Font::Unload() {
    glDeleteTextures(1, &this->texture);
    SDL_FreeSurface(this->image);

    this->loaded = false;
}

// TEXT CLASS //

