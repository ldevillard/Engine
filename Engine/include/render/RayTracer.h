#pragma once

#include <vector>

#include "component/Model.h"
#include "data/mesh/MeshData.h"
#include "render/Shader.h"
#include "render/ComputeShader.h"

struct RaytracingMaterial
{
	alignas(16) glm::vec3 Color = {};
	alignas(16) glm::vec3 EmissiveColor = {};
	float EmissiveStrength = 1;
};

struct alignas(16) RaytracingSphere
{
	alignas(16) glm::vec3 Position = {};
	float Radius = 1;
	RaytracingMaterial Material = {};
};

class RayTracer
{
public:
	static void Initialize(Shader* shader, ComputeShader* accumulate);
	static void Draw();

	static void ResetFrameCount();
	static unsigned int GetFrameCount();

private:
	static void setupScreenQuad();
	static std::vector<RaytracingSphere> getSceneData(const std::vector<Model*>& models);
	
	static unsigned int frameCount;
	static bool accumulate;

	static ScreenQuad screenQuad;
	static Shader* raytracingShader;
	static ComputeShader* accumulateShader;
	static GLuint ssbo;
};