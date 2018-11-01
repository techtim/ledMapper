#version 330

precision mediump float;
uniform mat4 modelViewProjectionMatrix;
uniform ivec2 outTexResolution;

layout(location = 0) in vec3 VertexPosition;
out vec2 ledPos;

void main()
{
    ledPos = VertexPosition.xy;
    gl_Position = modelViewProjectionMatrix
                  * vec4(gl_VertexID % outTexResolution.x, 1+floor(gl_VertexID / outTexResolution.x),
                         0.0, 1.0);
}