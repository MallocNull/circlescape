#include <SDL.h>
#include <GL/glew.h>
#include <emscripten.h>
#include <iostream>
#include <string>
#include "ui/font.hpp"

static struct {
    SDL_Window* window;
    SDL_GLContext gl_ctx;

    sosc::ui::Font* font;
    sosc::ui::Text* text;
} _ctx;

void draw();

int main(int argc, char** argv) {
    using namespace sosc;

    /*if(argc != 3)
        return -1;*/

    int client_width  = std::stoi(argv[1]),
        client_height = std::stoi(argv[2]);

    std::cout << "VERSION 9: " << argc << std::endl
              << argv[0] << std::endl
              << argv[1] << std::endl
              << argv[2] << std::endl;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _ctx.window = SDL_CreateWindow(
        "SockScape Client",
        0, 0,
        client_width, client_height,
        SDL_WINDOW_OPENGL
    );

    _ctx.gl_ctx = SDL_GL_CreateContext(_ctx.window);
    if(_ctx.gl_ctx == nullptr)
        return -1;

    if(glewInit() != GLEW_OK)
        return -1;

    ui::font_init_subsystem(_ctx.window);
    _ctx.font = new ui::Font(
        SOSC_RESC("fonts/scape.bmp"),
        SOSC_RESC("fonts/scape.dat")
    );
    ui::font_set_default(_ctx.font);

    _ctx.text = new ui::Text(
        75, glm::vec4(1, 0, 0, 1), "test text",
        100, 100, 400
    );

    emscripten_set_main_loop(draw, 0, 1);

    std::cout << "end test" << std::endl;
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(.25, .25, .25, 1);

    _ctx.text->Render();

    SDL_GL_SwapWindow(_ctx.window);
}