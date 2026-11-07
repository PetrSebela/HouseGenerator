#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

layout(binding=0) uniform samplerCube skybox;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragPosition;

void main() {
    vec3 view = normalize(frag.worldPosition);
    vec4 skyboxSample = texture(skybox, view);
    FragColor = skyboxSample;
    FragPosition = vec4(0, 1000, 0, 1);
}