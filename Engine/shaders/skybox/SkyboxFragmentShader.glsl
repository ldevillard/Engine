#version 430 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 lightColor;

void main()
{
    vec4 texColor = texture(skybox, TexCoords);

    FragColor = vec4(texColor.rgb * lightColor, texColor.a);
}