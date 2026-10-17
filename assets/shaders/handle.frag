#version 460
in vec3 vertColor;
in vec2 vertUV;
in vec3 fragWorldPosition;
out vec4 FragColor;

void main() {
    FragColor = vec4(0.25,0.9,0.9,1);
    float distance = length(vertUV - vec2(0.5,0.5));
    float offset = 0.1;
    float thickness = 0.01;
    if (distance < 0.05 || (distance > offset && distance < offset + thickness))
        return;
    discard;
}