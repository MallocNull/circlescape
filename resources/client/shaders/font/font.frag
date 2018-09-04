#version 330 core
in vec2 texCoords;
out vec4 fragColor;

uniform vec4 fontColor;
uniform sampler2D fontBitmap;

void main() {
    vec4 outColor = texture(fontBitmap, texCoords);
    if(outColor.xyz == vec3(0.0, 0.0, 0.0))
        discard;

    fragColor = fontColor * outColor;
}