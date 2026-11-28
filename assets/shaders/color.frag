#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 tangent;
    vec3 lightSpacePosition;
} frag;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragPosition;
layout(location=3) out vec4 PostProcessMask;

void main()
{
    FragColor = vec4(frag.color, 1);
    FragNormal = vec4(frag.normal, 1);
    FragPosition = vec4(frag.worldPosition, 1);
    PostProcessMask = vec4(1);
}