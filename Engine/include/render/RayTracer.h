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
	float Smoothness = 0;
};

struct RaytracingSphere
{
	alignas(16) glm::vec3 Position = {};
	float Radius = 1;

	RaytracingMaterial Material = {};
};

struct RaytracingCube
{
	alignas(16) glm::vec3 Min = {};
	alignas(16) glm::vec3 Max = {};
	alignas(16) glm::mat4 TransformMatrix = {};

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
	static void getSceneData(const std::vector<Model*>& models, std::vector<RaytracingSphere>& inout_spheres, std::vector<RaytracingCube>& inout_cubes);
	
	static unsigned int frameCount;
	static bool accumulate;

	static ScreenQuad screenQuad;
	static Shader* raytracingShader;
	static ComputeShader* accumulateShader;
	static GLuint sphereSSBO;
	static GLuint cubeSSBO;
};