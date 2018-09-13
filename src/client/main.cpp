#include <SDL.h>
#include <GL/glew.h>
#include <emscripten.h>
#include <iostream>

static struct {
    SDL_Window* window;
    SDL_GLContext gl_ctx;
} _ctx;

void draw();

int main(int argc, char** argv) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _ctx.window = SDL_CreateWindow(
        "SockScape Client",
        0, 0,
        640, 480,
        SDL_WINDOW_OPENGL
    );

    _ctx.gl_ctx = SDL_GL_CreateContext(_ctx.window);
    if(_ctx.gl_ctx == nullptr)
        return -1;

    if(glewInit() != GLEW_OK)
        return -1;

    emscripten_set_main_loop(draw, 0, 1);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(.25, .25, .25, 1);

    SDL_GL_SwapWindow(_ctx.window);
}