#version 460

in vec3 gizmoColor;
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragPosition;
layout(location=3) out vec4 PostProcessMask;

void main() {
    FragColor = vec4(gizmoColor, 1);
    FragNormal = vec4(0, 0, 0, 1);
    FragPosition = vec4(0, 0, 0, 1);
    PostProcessMask = vec4(0, 0, 0, 1);
}