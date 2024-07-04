#version 430 core

layout (location = 0) in vec3 aPos;

uniform vec3 cameraPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 FragPos;
out mat4 FragView;
out mat4 FragProj;

const float far = 2000;

void main()
{
    FragView = view;
    FragProj = projection;

    vec3 cameraCenteredVertexPos = vec3(aPos.x * far + cameraPos.x, -0.05f, -aPos.y * far + cameraPos.z); // -0.05 to avoid clipping

    FragPos = vec4(cameraCenteredVertexPos, 1.0f);
   
    gl_Position = projection * view * FragPos;
}