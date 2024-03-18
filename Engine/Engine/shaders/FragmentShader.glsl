#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

uniform Material material;
uniform sampler2D texture_diffuse1;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool wireframe;
uniform bool textured;

void ComputeLighting()
{
    // ambiant lighting
    vec3 ambient = lightColor * material.ambient;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * material.diffuse;

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    vec3 specular = spec * lightColor * material.specular;

    vec3 textureColor = vec3(1.0);

    if (textured)
    {
        textureColor = texture(texture_diffuse1, TexCoords).rgb;
    }

    vec3 result = (ambient + diffuse + specular) * objectColor * textureColor;
    FragColor = vec4(result, 1.0);
}

void main()
{
    if (!wireframe)
    {
        ComputeLighting();
    }
    else 
    {
        FragColor = vec4(material.diffuse, 1);
    }
}