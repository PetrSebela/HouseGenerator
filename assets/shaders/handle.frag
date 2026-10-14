#version 460
in vec3 vertColor;
in vec2 vertUV;
in vec3 fragWorldPosition;
out vec4 FragColor;

void main() {
    FragColor = vec4(1,1,1,1);

    if (length(vertUV - vec2(0.5,0.5)) > 0.1)
        discard;
}