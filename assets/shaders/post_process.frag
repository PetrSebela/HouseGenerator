#version 460
in vec2 UV;
out vec4 FragColor;
uniform sampler2D color;

void main() {
    FragColor = vec4(UV,0,1);
    FragColor = texture(color, UV);
}