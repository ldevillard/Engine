#pragma once

#include "data/Color.h"
#include "render/Shader.h"
#include "render/ComputeShader.h"

class Outliner 
{
public:
    static void Initialize(Shader* outlineShader, Shader* outlineDilatingShader, ComputeShader* outlineBlitShader);
    
    static Shader* OutlineShader;
    static Shader* OutlineDilatingShader;
    static ComputeShader* OutlineBlitShader;
    static float OutlineWidth;
    static Color OutlineColor;
};