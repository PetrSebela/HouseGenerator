#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

layout(binding=0) uniform sampler2D skybox;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragPosition;

const float PI = 3.14159265359;

// ChatGPU special, fixes atan at x = 0
float atan2Safe(float y, float x)
{
    if (abs(x) >= abs(y)) {
        float a = atan(y / x);
        if (x < 0.0) a += (y >= 0.0) ? PI : -PI;
        return a;
    } else {
        return sign(y) * (0.5 * PI) - atan(x / y);
    }
}

void main() {
    vec3 view = normalize(frag.worldPosition);

    float u = atan2Safe(view.x, view.z) / (2.0 * PI) + 0.5;
    float v = asin(clamp(view.y, -1.0, 1.0)) / PI + 0.5;

    vec2 samplePosition = vec2(u, v);
    vec4 skyboxSample = texture(skybox, samplePosition);

    FragColor = skyboxSample;
    FragPosition = vec4(0, 1000, 0, 1);
}