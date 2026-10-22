#version 460
in vec3 vertColor;
in vec2 vertUV;
in vec3 fragWorldPosition;
out vec4 FragColor;

void main() {
    FragColor = vec4(0.0, 0.1, 0.5, 1.0);
    float distance = length(vertUV - vec2(0.5,0.5));
    float offset = 0.1;
    float thickness = 0.02;

    if (distance < 0.05/2 || (distance > offset && distance < offset + thickness))
        return;
    discard;
}