#version 460
in struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

out vec4 FragColor;
uniform sampler2D tex;

void main()
{
    FragColor = pow(texture(tex, frag.uv),vec4(1.2f));
}