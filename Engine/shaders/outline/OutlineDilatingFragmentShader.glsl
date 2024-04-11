#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float radius;

void main()
{
    float radius = 3.0; // radius of the dilation

    vec3 dilatedColor = vec3(0.0);
    float stepX = 1.0 / float(textureSize(screenTexture, 0).x); // calculate step size based on texture dimensions
    float stepY = 1.0 / float(textureSize(screenTexture, 0).y);

    // Loop through neighbors within the radius
    for (float i = -radius; i <= radius; i++) 
    {
        for (float j = -radius; j <= radius; j++) 
        {
            vec2 offset = vec2(i * stepX, j * stepY);
            vec3 color = texture(screenTexture, TexCoords + offset).rgb;
            dilatedColor = max(dilatedColor, color);
        }
    }

    FragColor = vec4(dilatedColor, 1.0);
}