#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform float radius;
uniform sampler2D screenTexture;

void main()
{
    vec3 dilatedColor = vec3(0.0);
    float stepX = 1.0 / float(textureSize(screenTexture, 0).x); // calculate step size based on texture dimensions
    float stepY = 1.0 / float(textureSize(screenTexture, 0).y);

    // check if any neighbor is not part of the outline
    vec4 neighbourUp = texture(screenTexture, TexCoords + vec2(0.0, stepY));
    vec4 neighbourDown = texture(screenTexture, TexCoords + vec2(0.0, -stepY));
    vec4 neighbourLeft = texture(screenTexture, TexCoords + vec2(-stepX, 0.0));
    vec4 neighbourRight = texture(screenTexture, TexCoords + vec2(stepX, 0.0));

    if (neighbourUp.r < 0.5 || neighbourDown.r < 0.5 || neighbourLeft.r < 0.5 || neighbourRight.r < 0.5)
    {
        // At least one neighbor is not part of the outline, perform dilation
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
    else
    {
        // All neighbors are part of the outline, draw black
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}