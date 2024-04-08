#pragma once

#include "data/Color.h"
#include "render/Shader.h"

class Outliner 
{
public:
    static void Initialize(Shader* outlineShader);
    
    static Shader* OutlineShader;
    static float OutlineWidth;
    static Color OutlineColor;
};