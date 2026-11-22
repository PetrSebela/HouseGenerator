#version 460

uniform mat4 view = mat4(1);
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec3 gizmoColor;

void main() {
    gl_Position = view * vec4(position, 1);
    gizmoColor = color;
}