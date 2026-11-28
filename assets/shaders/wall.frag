#version 460

layout(binding=0) uniform sampler2D shadowMap;
layout(binding=1) uniform sampler2D albedoMap;
layout(binding=2) uniform sampler2D normalMap;

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

void main() {
    vec3 bitangent = cross(frag.tangent, frag.normal);
    mat3 TBN = mat3(frag.tangent, bitangent, frag.normal);
    vec3 normal = texture(normalMap, frag.uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 mappedNormal = TBN * normal;

    FragColor = pow(texture(albedoMap, frag.uv),vec4(1.6));
    FragNormal = vec4(mappedNormal, 1);
    FragPosition = vec4(frag.worldPosition, 1);
    PostProcessMask = vec4(1);
}