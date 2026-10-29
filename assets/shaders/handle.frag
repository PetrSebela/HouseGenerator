#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragPosition;

void main() {
    FragColor = vec4(frag.color, 1);
    FragNormal = vec4(frag.normal, 1);
    FragPosition = vec4(frag.worldPosition, 1);

    float distance = length(frag.uv - vec2(0.5,0.5));
    float offset = 0.1;
    float thickness = 0.02;

    if (distance < 0.05/2 || (distance > offset && distance < offset + thickness))
        return;
    discard;
}