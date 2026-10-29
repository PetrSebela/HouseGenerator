#version 460
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform mat4 view = mat4(1);
uniform mat4 model = mat4(1);
uniform mat4 light = mat4(1);

out struct VertexOut{
    vec3 color;
    vec2 uv;
    vec3 worldPosition;
    vec3 normal;
    vec3 lightSpacePosition;
} frag;

void main()
{
    gl_Position = view * model * vec4(position, 1.0);

    frag.worldPosition = (model * vec4(position, 1.0)).xyz;
    frag.color = vec3(0,0.1,0.5);
    frag.uv = uv;
    frag.normal = normal;
    frag.lightSpacePosition = (light * model * vec4(position, 1.0)).xyz;
}