#version 460
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;

uniform mat4x4 view;
uniform mat4x4 model;

out vec3 vertColor;
out vec2 vertUV;

void main()
{
    gl_Position = view * model * vec4(position.x, position.y, position.z, 1.0);
    vertColor = color;
    vertUV = uv;
}