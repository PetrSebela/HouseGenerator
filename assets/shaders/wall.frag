#version 460

layout(binding=0) uniform sampler2D shadowMap;
layout(binding=1) uniform sampler2D albedoMap;
layout(binding=2) uniform sampler2D normalMap;

in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

//in vec3 fragWorldPosition;
//in vec3 fragLightPosition;
//in vec2 vertUV;
//in vec3 vertNormal;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragPosition;

float shadow(vec3 lightPos)
{
    if (abs(lightPos.x) > 1 || abs(lightPos.y) > 1)
        return 0;

    float bias = 0.0001;
    vec2 texCoords = (lightPos.xy + vec2(1,1) ) / 2;
    if((lightPos.z + 1) / 2 > texture(shadowMap, texCoords.xy).r + bias)
        return 1;
    return 0;
}

void main() {

//    THIS IS TEMPORARY, CONSTRUCT PROPPER TBN MATRIX WHEN REFACTORING!!!!!
    vec3 tangent = vec3(0,1,0);
    vec3 bitangent = cross(tangent, frag.normal);
    mat3 TBN = mat3(tangent, bitangent, frag.normal);
    vec3 normal = texture(normalMap, frag.uv).rgb;
    normal = -normalize(normal * 2.0 - 1.0);
    vec3 mappedNormal = TBN * normal;
    float intensity = max(dot(mappedNormal, normalize(-vec3(1,1,1))), 0) * (1 - shadow(frag.lightSpacePosition));
    float ambient = 0.25 + 0.25 * intensity;
    vec4 albedo = texture(albedoMap, frag.uv);

    FragColor = pow(texture(albedoMap, frag.uv),vec4(1.6));
    FragNormal = vec4(mappedNormal, 1);
    FragPosition = vec4(frag.worldPosition, 1);
}