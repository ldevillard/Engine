#version 430 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout(binding = 0, rgba8) uniform image2D accumulateTexture;
layout(binding = 1, rgba8) uniform image2D raytracedTexture;

uniform uint frameCount;

void main()
{
    ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

    float weight = 1.0 / float(frameCount + 1);

    vec4 raytracedColor = imageLoad(raytracedTexture, texelCoords);
    vec4 accumulatedColor = imageLoad(accumulateTexture, texelCoords);

    vec4 result = clamp(accumulatedColor * (1.0 - weight) + raytracedColor * weight, 0.0, 1.0);
    
    imageStore(accumulateTexture, texelCoords, result);
}