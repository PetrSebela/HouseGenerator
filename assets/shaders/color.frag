#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

out vec4 FragColor;

void main()
{
    FragColor = vec4(frag.color, 1);
}