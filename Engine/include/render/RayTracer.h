#pragma once

#include "data/mesh/MeshData.h"

#include "render/Shader.h"

class RayTracer
{
public:
	static void Initialize(Shader* shader);
	static void Draw();

private:
	static void setupScreenQuad();
	static ScreenQuad screenQuad;
	static Shader* raytracingShader;
};