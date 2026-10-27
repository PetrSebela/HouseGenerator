#version 460

layout(binding=0) uniform sampler2D shadowMap;
layout(binding=1) uniform sampler2D albedoMap;
layout(binding=2) uniform sampler2D normalMap;

in vec3 fragWorldPosition;
in vec3 fragLightPosition;
in vec2 vertUV;
in vec3 vertNormal;

out vec4 FragColor;

float shadow(vec3 lightPos)
{
    if (abs(lightPos.x) > 1 || abs(lightPos.y) > 1)
    return 0;

    float bias = 0.00001;
    vec2 texCoords = (lightPos.xy + vec2(1,1) ) / 2;
    if((lightPos.z - bias + 1) / 2 > texture(shadowMap, texCoords.xy).r)
        return 1;
    return 0;
}

void main() {

//    THIS IS TEMPORARY, CONSTRUCT PROPPER TBN MATRIX WHEN REFACTORING!!!!!
    vec3 tangent = vec3(0,1,0);
    vec3 bitangent = cross(tangent, vertNormal);
    mat3 TBN = mat3(tangent, bitangent, vertNormal);
    vec3 normal = texture(normalMap, vertUV).rgb;
    normal = normalize(normal * 2.0 - 1.0) * 3;
    vec3 mappedNormal = TBN * normal;
    float intensity = max(dot(mappedNormal, normalize(-vec3(1,1,1))), 0) * (1 - shadow(fragLightPosition));
    float ambient = 0.25 + 0.25 * intensity;
    vec4 albedo = texture(albedoMap, vertUV);

//  texture is in sRGB, doing this to avoid double gamma correction
    vec3 color = pow(albedo.rgb, vec3(1.6)) * ambient;
    FragColor = vec4(color, 1);
}