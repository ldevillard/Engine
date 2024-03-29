#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

#define MAX_LIGHTS_COUNT 8

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct Light 
{
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;

    // point light
    float radius;

    // spot light
    float cutOff;
    float outCutOff;
};


uniform Light     lights[MAX_LIGHTS_COUNT];
uniform int       lightsCount;
uniform Material  material;
uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;

uniform bool wireframe;
uniform bool textured;
uniform bool blinn;

vec3 ComputeDirectionalLighting(Light light)
{
    // ambiant lighting
    vec3 ambient = light.color * material.ambient;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * material.diffuse;

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, reflectDir), 0.0), material.shininess * 128);
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    }
    vec3 specular = spec * light.color * material.specular;

	return (ambient + diffuse + specular) * light.intensity;
}

vec3 ComputePointLighting(Light light)
{
    float distance = length(light.position - FragPos);
    float radius = light.radius;
    float constant = 1.0;
    float linear = 2.0 / (radius * 0.7);
    float quadratic = 1.0 / (radius * radius);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // ambiant lighting
    vec3 ambient = light.color * material.ambient * attenuation;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * material.diffuse * attenuation;

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, reflectDir), 0.0), material.shininess * 128);
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    }
    vec3 specular = spec * light.color * material.specular * attenuation;

    return (ambient + diffuse + specular) * light.intensity;
}

vec3 ComputeSpotLighting(Light light)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    // outerCutOff is used to make a fade edge of the spot
    float epsilon = light.cutOff - light.outCutOff;
    float intensity = clamp((theta - light.outCutOff) / epsilon, 0.0, 1.0);

    // attenuation
    float distance = length(light.position - FragPos);
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);


    // ambiant lighting
    vec3 ambient = light.color * material.ambient * attenuation * intensity;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * material.diffuse * intensity * attenuation;

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, reflectDir), 0.0), material.shininess * 128);
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
    }
    vec3 specular = spec * light.color * material.specular * intensity * attenuation;

    return (ambient + diffuse + specular) * light.intensity;
}

vec3 ComputeLighting(Light light)
{
    if (light.type == 0)
    {
        return ComputeDirectionalLighting(light);
	}
    else if (light.type == 1)
    {
		return ComputePointLighting(light);
	}
    else if (light.type == 2)
    {
		return ComputeSpotLighting(light);
    }
}

void main()
{
    if (!wireframe)
    {
        vec3 computedLight = vec3(0.0);

        for (int i = 0; i < lightsCount; i++)
        {
            computedLight += ComputeLighting(lights[i]);
        }

        vec3 textureColor = vec3(1.0);

        if (textured)
        {
            textureColor = texture(texture_diffuse1, TexCoords).rgb;
        }
        
        FragColor = vec4(computedLight * textureColor, 1.0);
    }
    else if (textured)
    {
		FragColor = texture(texture_diffuse1, TexCoords);
	}
	else
    {
        FragColor = vec4(material.diffuse, 1);
    }

    //FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}