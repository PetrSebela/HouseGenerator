#version 460
layout(binding=0) uniform sampler2D shadowMap;

in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

out vec4 FragColor;

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

// Rewrite of https://bgolus.medium.com/the-best-darn-grid-shader-yet-727f9278b9d8
float grid(vec2 uv, vec2 targetWidth)
{
    vec4 uvDDXY = vec4(dFdx(uv),dFdy(uv));
    vec2 uvDeriv = vec2(length(uvDDXY.xz), length(uvDDXY.yw));
    vec2 drawWidth = clamp(targetWidth, uvDeriv, vec2(0.5,0.5));
    vec2 lineAA = max(uvDeriv, 0.000001) * 1.5;
    vec2 gridUV = abs(fract(uv) * 2.0 - 1.0);

    vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
    grid2 *= clamp(targetWidth / drawWidth,0,1);
    grid2 = mix(grid2, targetWidth, clamp(uvDeriv * 2.0 - 1.0, 0, 1));
    return mix(grid2.x, 1.0, grid2.y);
}

void main() {
    float ambient = 0.2 + 0.3 * (1 - shadow(frag.lightSpacePosition));

    vec2 majorWidth = vec2(0.0175, 0.0175);
    vec2 majorUV = frag.worldPosition.xz / 10 + vec2(0.5,0.5);
    vec2 minorUV = frag.worldPosition.xz + vec2(0.5,0.5);

    float major = grid(majorUV, majorWidth / 10);
    float minor = grid(minorUV, majorWidth);


    vec3 majorColor = mix(vec3(1), vec3(2), major) * ambient;
    vec3 minorColor = mix(vec3(1), vec3(2), minor) * ambient;

    FragColor = vec4(max(majorColor, minorColor), 1);
//    FragColor = vec4(ambient, ambient, ambient, 1);
}