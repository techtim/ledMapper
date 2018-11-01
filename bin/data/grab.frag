#version 330

uniform sampler2DRect texIn;
in vec2 ledPos;
out vec4 vFragColor;

void main()
{
    vFragColor = texture(texIn, ledPos);
}
