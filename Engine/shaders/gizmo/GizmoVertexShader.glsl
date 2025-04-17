#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords; // maybe useless but keep it for now
layout(location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int instanceEnabled;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    if (instanceEnabled != 0)
    {
        gl_Position *= instanceMatrix;
    }

    FragPos = vec3(model * vec4(aPos, 1));
    Normal = vec3(model * vec4(aNormal, 0));
    TexCoords = aTexCoords;
}