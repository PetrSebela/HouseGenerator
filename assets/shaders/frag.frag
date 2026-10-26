#version 460
in vec3 vertColor;
in vec2 vertUV;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    FragColor = pow(texture(tex, vertUV),vec4(1.2f));
}