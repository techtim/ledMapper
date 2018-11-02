#version 330

uniform sampler2DRect texIn;
in vec2 ledPos;
out vec4 vFragColor;

vec4 colorConvert(vec4 col) {
    return vec4(col.g, col.b, col.r, 1.0);
}

void main()
{
    vFragColor = colorConvert(texture(texIn, ledPos));
}

