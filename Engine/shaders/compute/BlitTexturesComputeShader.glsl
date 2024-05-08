#version 430 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout(binding = 0, rgba16) uniform image2D sceneTexture;
layout(binding = 1, rgba16) uniform image2D outlineTexture;

void main()
{
    ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

    vec4 rgba = imageLoad(outlineTexture, texelCoords);

    if (rgba.r > 0.5)
    {
        imageStore(sceneTexture, texelCoords, rgba);
    }
}