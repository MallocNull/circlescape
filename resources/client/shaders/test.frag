#version 330 core
in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D fontBitmap;

void main() {
    fragColor = texture(fontBitmap, texCoords);
}