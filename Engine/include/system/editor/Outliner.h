#pragma once

#include "data/Color.h"
#include "render/Shader.h"

class Outliner 
{
public:
    static void Initialize(Shader* outlineShader, Shader* outlineDilatingShader, Shader* outlineBlitShader);
    
    static Shader* OutlineShader;
    static Shader* OutlineDilatingShader;
    static Shader* OutlineBlitShader;
    static float OutlineWidth;
    static Color OutlineColor;
};