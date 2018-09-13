precision mediump float;

varying vec2 texCoords;

uniform vec4 fontColor;
uniform sampler2D fontBitmap;

void main() {
    vec4 outColor = texture2D(fontBitmap, texCoords);
    if(outColor.xyz == vec3(0.0, 0.0, 0.0))
        discard;

    gl_FragColor = fontColor * outColor;
}