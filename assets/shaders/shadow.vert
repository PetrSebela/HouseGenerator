#version 460

layout (location=0) in vec3 position;
uniform mat4 light = mat4(1);
uniform mat4 model = mat4(1);

void main() {
    gl_Position = light * model * vec4(position, 1);
}