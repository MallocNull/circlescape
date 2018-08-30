#version 330 core
layout (location = 0) in vec2 aScreenCoords;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 transMatrix;
uniform mat4 orthoMatrix;

void main() {
    gl_Position = orthoMatrix * transMatrix * vec4(aScreenCoords, 0.0, 1.0);
    texCoords = aTexCoords;
}