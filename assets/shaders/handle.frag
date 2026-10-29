#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

out vec4 FragColor;

void main() {
    FragColor = vec4(frag.color, 1);
    float distance = length(frag.uv - vec2(0.5,0.5));
    float offset = 0.1;
    float thickness = 0.02;

    if (distance < 0.05/2 || (distance > offset && distance < offset + thickness))
        return;
    discard;
}