attribute vec2 aScreenCoords;
attribute vec2 aTexCoords;

varying vec2 texCoords;

uniform mat4 transMatrix;
uniform mat4 orthoMatrix;

void main() {
    gl_Position = orthoMatrix * transMatrix * vec4(aScreenCoords, 1.0, 1.0);
    texCoords = aTexCoords;
}