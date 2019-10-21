#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <GL/glew.h>
#include <emscripten.h>

static struct {
    SDL_Window* hwnd;
    SDL_GLContext gl;
} _ctx;

void draw();

int main(int argc, char** argv) {
    if(argc != 3)
        return -1;

    int client_width  = atoi(argv[1]),
        client_height = atoi(argv[2]);

    printf("VERSION 1: %i \r\n"
           "%s\r\n%s\r\n%s\r\n",
           argc, argv[0], argv[1], argv[2]);

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("%s\r\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _ctx.hwnd = SDL_CreateWindow(
        "SockScape Client",
        0, 0,
        client_width, client_height,
        SDL_WINDOW_OPENGL
    );

    _ctx.gl = SDL_GL_CreateContext(_ctx.hwnd);
    if(_ctx.gl == NULL)
        return -1;

    if(glewInit() != GLEW_OK)
        return -1;

    EM_ASM(setup_resize_event());

    emscripten_set_main_loop(draw, 0, 1);

    SDL_GL_DeleteContext(_ctx.gl);
    SDL_DestroyWindow(_ctx.hwnd);
    return 0;
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(.25, .25, .25, 1);

    SDL_GL_SwapWindow(_ctx.hwnd);

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_WINDOWEVENT &&
           event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            glViewport(0, 0, event.window.data1, event.window.data2);
        }
    }
}

void resize_context(int width, int height) {
    SDL_SetWindowSize(_ctx.hwnd, width, height);
}