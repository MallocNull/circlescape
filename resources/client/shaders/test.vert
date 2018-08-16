#version 330 core
layout (location = 0) in vec2 aScreenCoords;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

uniform vec2 screenSize;

void main() {
    mat2 scMatrix = mat2(screenSize.x);
    vec2 viewCoords = scMatrix * aScreenCoords;
    gl_Position =
        vec4((2 * aScreenCoords.x) / screenSize.x - 1,
             1 - (2 * aScreenCoords.y) / screenSize.y,
             0, 0);

    texCoords = aTexCoords;
}