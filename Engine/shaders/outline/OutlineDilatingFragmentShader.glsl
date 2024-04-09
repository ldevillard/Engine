#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float radius;
uniform float gridX; // 1/screenWidth
uniform float gridY; // 1/screenHeight

void main() {
    vec3 dilatedColor = vec3(0.0);

    // Sample the texture in a grid around the current pixel
    float startI = max(-radius, -TexCoords.x / gridX);
    float endI = min(radius, (1.0 - TexCoords.x) / gridX);
    float startJ = max(-radius, -TexCoords.y / gridY);
    float endJ = min(radius, (1.0 - TexCoords.y) / gridY);

    for (float i = startI; i <= endI; ++i) 
    {
        for (float j = startJ; j <= endJ; ++j) 
        {
            vec2 uv = TexCoords.st + vec2(i * gridX, j * gridY);
            vec3 color = texture(screenTexture, uv).rgb;
            dilatedColor = max(dilatedColor, color); // Dilatation
        }
    }

    FragColor = vec4(dilatedColor, 1.0);
}