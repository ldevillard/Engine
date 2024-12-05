#pragma once

#include <string>
#include <vector>

#include "component/Model.h"
#include "data/mesh/MeshData.h"
#include "data/template/Singleton.h"
#include "render/Shader.h"
#include "render/ComputeShader.h"

class CubeMap;

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

struct RaytracingTriangle
{
	alignas(16) glm::vec3 A = {};
	alignas(16) glm::vec3 B = {};
	alignas(16) glm::vec3 C = {};
	
	alignas(16) glm::vec3 NormalA = {};
	alignas(16) glm::vec3 NormalB = {};
	alignas(16) glm::vec3 NormalC = {};

	alignas(8) glm::vec2 UVA = {};
	alignas(8) glm::vec2 UVB = {};
	alignas(8) glm::vec2 UVC = {};
};

struct RaytracingMesh
{
	int FirstTriangleIndex = 0;
	int FirstNodeIndex = 0;

	alignas(16) glm::mat4 TransformMatrix = {};
	alignas(16) glm::mat4 InverseTransformMatrix = {};

	RaytracingMaterial Material = {};
};

struct RaytracingBVHNode
{
	alignas(16) glm::vec3 BoundsMin = {};
	alignas(16) glm::vec3 BoundsMax = {};
	int TriangleIndex = 0;
	int TriangleCount = 0;
	int ChildIndex = 0;
};

class Raytracer : public Singleton<Raytracer>
{
public:
	void Initialize(Shader* shader, ComputeShader* accumulate);
	void Draw(const CubeMap& cubeMap);

	void ResetFrameCount();
	unsigned int GetFrameCount();

protected:
	void initialize() override;

private:
	void setupScreenQuad();
	void getSceneData(const std::vector<Model*>& models, std::vector<RaytracingSphere>& inout_spheres, std::vector<RaytracingCube>& inout_cubes,
							 std::vector<RaytracingTriangle>& inout_triangles, std::vector<RaytracingMesh>& inout_meshes,
							 std::vector<RaytracingBVHNode>& inout_nodes, std::vector<GLuint64>& inout_handles);
	
	unsigned int frameCount = 0;
	bool accumulate = false;

	std::map<std::string, std::vector<RaytracingTriangle>> meshesTriangles = {};
	std::map<std::string, std::vector<RaytracingBVHNode>> meshesNodes = {};
	int meshCount = 0;

	ScreenQuad screenQuad = {};
	Shader* raytracingShader = 0;
	ComputeShader* accumulateShader = nullptr;

	GLuint sphereSSBO = 0;
	GLuint cubeSSBO = 0;
	GLuint triangleSSBO = 0;
	GLuint meshSSBO = 0;
	GLuint bvhSSBO = 0;
	GLuint textureSSBO = 0;
};