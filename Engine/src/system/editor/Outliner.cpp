#include "system/editor/Outliner.h"

// init static members but initialize them also in the Initialize 
// method because Color might not be initialized yet
Shader* Outliner::OutlineShader = nullptr;
Shader* Outliner::OutlineDilatingShader = nullptr;
Shader* Outliner::OutlineBlitShader = nullptr;
Color Outliner::OutlineColor = glm::vec3(1.f, 0.565f, 0.161f);
float Outliner::OutlineWidth = 0.04f;

#pragma region Static Methods

void Outliner::Initialize(Shader* outlineShader, Shader* outlineDilatingShader, Shader* outlineBlitShader)
{
	OutlineShader = outlineShader;
	OutlineDilatingShader = outlineDilatingShader;
	OutlineBlitShader = outlineBlitShader;
}

#pragma endregion