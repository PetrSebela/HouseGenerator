#version 460

layout (location=0) in vec3 position;
uniform mat4 lightSpaceTransform = mat4(1);
uniform mat4 model = mat4(1);

void main() {
    gl_Position = lightSpaceTransform * model * vec4(position, 1);
}