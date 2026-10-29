#version 460
in vec2 UV;
out vec4 FragColor;

layout(binding=0) uniform sampler2D albedoMap;
layout(binding=1) uniform sampler2D normalMap;
layout(binding=2) uniform sampler2D positionMap;
layout(binding=3) uniform sampler2D shadowMap;
uniform mat4 lightSpaceTransform = mat4(1);

float inShadow(vec3 worldPos)
{
    vec3 lightPos = (lightSpaceTransform * vec4(worldPos, 1)).xyz;
    if (abs(lightPos.x) > 0.99 || abs(lightPos.y) > 0.99)
        return 1;

    float bias = 0.00001;
    vec2 texCoords = (lightPos.xy + vec2(1,1)) / 2;
    if((lightPos.z + 1) / 2 > texture(shadowMap, texCoords.xy).r + bias)
        return 0;
    return 1;
}

void main() {
    vec3 albedo = texture(albedoMap, UV).xyz;
    vec3 position = texture(positionMap, UV).xyz;
    vec3 normal = texture(normalMap, UV).xyz;

    // For now zero vector serves as flag that there isnt any geometry, I am too lazy to bind depth buffer
    if (length(normal) < 0.5) {
        FragColor = vec4(0.5,0.5,0.5, 1);
        return;
    }

    normal = normalize(normal);
    vec3 normalDebug = normal * 0.5 + 0.5;
    vec3 sunDirection = normalize(vec3(9,12,15));

    float intensity = inShadow(position) * max(dot(normal, sunDirection), 0);
    vec3 shaded = albedo * mix(0.25, 1, intensity);

    FragColor = vec4(shaded, 1);
//    FragColor = vec4(normalDebug, 1);
}