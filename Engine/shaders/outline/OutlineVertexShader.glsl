#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlining;

void main()
{
    // Transform the vertex position to world space
    vec4 worldPos = model * vec4(aPos, 1.0);

    // Calculate the outline position in world space
    vec3 normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 viewDir = normalize((view * worldPos).xyz);
    vec3 outlineDir = mix(normal, viewDir, outlining); // Adjust outlining factor here
    vec4 outlinePos = worldPos + vec4(outlineDir * outlining, 0.0);

    // Transform the outline position to clip space
    gl_Position = projection * view * outlinePos;
}