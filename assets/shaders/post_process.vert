#version 460

layout (location=0) in vec2 inPos;
layout (location=3) in vec2 inUV;
out vec2 UV;


void main() {
    gl_Position = vec4(inPos,0,1);
    UV = inUV;
}