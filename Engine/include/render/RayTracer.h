#pragma once

#include <vector>

#include "component/Model.h"
#include "data/mesh/MeshData.h"
#include "render/Shader.h"

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
	static void Initialize(Shader* shader);
	static void Draw();

private:
	static void setupScreenQuad();
	static std::vector<RaytracingSphere> getSceneData(const std::vector<Model*>& models);
	
	static ScreenQuad screenQuad;
	static Shader* raytracingShader;
	static GLuint ssbo;
};