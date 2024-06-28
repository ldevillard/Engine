#version 430 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 lightDirection;
uniform float lightIntensity;

const vec3 dayColor = vec3(1.0, 1.0, 1.0);
const vec3 nightColor = vec3(0.1, 0.1, 0.15);

void main()
{
    vec4 texColor = texture(skybox, TexCoords);

    float lightAngleT = clamp((lightDirection.y + 1.0) * 0.5, 0.0, 1.0);
    vec3 finalColor = mix(dayColor, nightColor, lightAngleT) * (1 + log(lightIntensity));

    FragColor = vec4(texColor.rgb * finalColor, texColor.a);
}