#version 430 core

out vec4 FragColor;

in vec4 FragPos;
in mat4 FragView;
in mat4 FragProj;

const float near = 0.01f;
const float far = 20.0f;

vec4 Grid(vec3 fragPos3D, float scale, bool drawAxis)
{
    vec2 coord = fragPos3D.xz / scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);

    vec2 grid = abs(fract(coord - 0.5f) - 0.5f) / derivative;
    float line = min(grid.x, grid.y);

    vec4 color = vec4(0.1f, 0.1f, 0.1f, 1.0f - min(line, 1.0f));

    // TODO: take drawAxis and scale into account here
    if (drawAxis)
    {
        // z axis
        float minimumX = min(derivative.x, 1.0f);
        if (fragPos3D.x > -1.5f * scale * minimumX && fragPos3D.x < 0.5f * scale * minimumX)
        {
            color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        }

        // x axis
        float minimumZ = min(derivative.y, 1.0f);
        if (fragPos3D.z > -1.5f * scale * minimumZ && fragPos3D.z < 0.5f * scale * minimumZ)
        {
            color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }

    // Reset the color's RGB if its alpha means it is not visible (allows to add grids without whitening the grid color)
    return color.a > 0.01f
        ? color
        : vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

float ComputeLinearDepth(vec3 pos) 
{
    vec4 clipSpacePos = FragProj * FragView * vec4(pos, 1.0f);
    float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0f * near * far) / (far + near - clipSpaceDepth * (far - near)); // get linear value between near and far
    return linearDepth / far; // normalize
}

void main()
{
    // Generate a first 1x1 grid
    FragColor = Grid(FragPos.xyz, 1.0f, false);
   
    // Fade the 1x1 intensity out (adjust as needed)
    FragColor.a *= 0.2f;
   
    // Generate a second 10x10 grid and add it to the first
    FragColor += Grid(FragPos.xyz, 10.0f, true);
   
    float linearDepth = ComputeLinearDepth(FragPos.xyz);
   
    // Fade de grid out as it get farther from the camera (adjust as needed)
    float maxAlpha = 0.7f;
    float fading = max(0.0f, maxAlpha - linearDepth);

    FragColor.a *= fading;
}
