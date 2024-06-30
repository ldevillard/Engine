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
	alignas(16) glm::vec3 SpecularColor = {};
	int Flag = 0;
	float EmissiveStrength = 1;
	float Smoothness = 0;
	float SpecularProbability = 1;
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
	alignas(16) glm::mat4 InverseTransformMatrix = {};

	RaytracingMaterial Material = {};
};

struct RayTracingTriangle
{
	alignas(16) glm::vec3 A = {};
	alignas(16) glm::vec3 B = {};
	alignas(16) glm::vec3 C = {};
	
	alignas(16) glm::vec3 NormalA = {};
	alignas(16) glm::vec3 NormalB = {};
	alignas(16) glm::vec3 NormalC = {};
};

struct RayTracingMesh
{
	int FirstTriangleIndex = 0;
	int FirstNodeIndex = 0;

	alignas(16) glm::mat4 InverseTransformMatrix = {};

	RaytracingMaterial Material = {};
};

struct RayTracingBVHNode
{
	alignas(16) glm::vec3 BoundsMin = {};
	alignas(16) glm::vec3 BoundsMax = {};
	int TriangleIndex = 0;
	int TriangleCount = 0;
	int ChildIndex = 0;
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
	static void getSceneData(const std::vector<Model*>& models, std::vector<RaytracingSphere>& inout_spheres, std::vector<RaytracingCube>& inout_cubes,
							 std::vector<RayTracingTriangle>& inout_triangles, std::vector<RayTracingMesh>& inout_meshes,
							 std::vector<RayTracingBVHNode>& inout_nodes);
	
	static unsigned int frameCount;
	static bool accumulate;

	static ScreenQuad screenQuad;
	static Shader* raytracingShader;
	static ComputeShader* accumulateShader;
	static GLuint sphereSSBO;
	static GLuint cubeSSBO;
	static GLuint triangleSSBO;
	static GLuint meshSSBO;
	static GLuint bvhSSBO;
};