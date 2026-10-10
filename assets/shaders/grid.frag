#version 460
in vec3 vertColor;
in vec2 vertUV;
in vec3 fragWorldPosition;
out vec4 FragColor;

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

void main()
{
    vec2 majorWidth = vec2(0.0175, 0.0175);

    vec2 majorUV = fragWorldPosition.xz / 10 + vec2(0.5,0.5);
    vec2 minorUV = fragWorldPosition.xz + vec2(0.5,0.5);

    float major = grid(majorUV, majorWidth / 10);
    float minor = grid(minorUV, majorWidth);

    vec4 majorColor = vec4(1,1,1,1) * major;
    vec4 minorColor = vec4(0.5,0.5,0.5,1) * minor;

    FragColor = majorColor + minorColor;
}