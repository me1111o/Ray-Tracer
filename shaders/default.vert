#version 330 core


layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 projection;
uniform mat4 modelview;

uniform mat3 normalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    vec4 pos = modelview * vec4(vPosition, 1.0);
    FragPos = pos.xyz;
    Normal = normalize(normalMatrix * vNormal);
    TexCoord = vTexCoord;
    gl_Position = projection * pos;
}
