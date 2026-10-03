#version 330
in vec3 vertColor;
in vec2 vertUV;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, vertUV);
}