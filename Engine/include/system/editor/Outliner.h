#pragma once

#include "data/Color.h"
#include "data/mesh/MeshData.h"

#include "render/Shader.h"
#include "render/ComputeShader.h"

class Outliner 
{
public:
    static void Initialize(Shader* outlineShader, Shader* outlineDilatingShader, ComputeShader* outlineBlitShader);
    
    static void Setup();
    static void Draw();
    static void Reset();

    static Shader* OutlineShader;
    static Shader* OutlineDilatingShader;
    static ComputeShader* OutlineBlitShader;
    static float OutlineWidth;
    static Color OutlineColor;

private:
    static void setupScreenQuad();

    static ScreenQuad screenQuad;
    static float radius;
};