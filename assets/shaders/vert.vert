#version 460
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform mat4 view = mat4(1);
uniform mat4 model = mat4(1);
uniform mat4 light = mat4(1);

out vec3 vertColor;
out vec2 vertUV;
out vec3 fragWorldPosition;
out vec3 vertNormal;
out vec3 fragLightPosition;

void main()
{
    gl_Position = view * model * vec4(position, 1.0);
    fragWorldPosition = (model * vec4(position, 1.0)).xyz;
    vertColor = vec3(uv,0);
    vertUV = uv;
    vertNormal = normal;
    fragLightPosition = (light * model * vec4(position, 1.0)).xyz;
}