#pragma once

#include <vector>

#include "component/Model.h"
#include "data/mesh/MeshData.h"
#include "render/Shader.h"

struct alignas(16) RaytracingData
{
	Sphere sphere;
	glm::vec3 color;
};

class RayTracer
{
public:
	static void Initialize(Shader* shader);
	static void Draw();

private:
	static void setupScreenQuad();
	static std::vector<RaytracingData> getSceneData(const std::vector<Model*>& models);
	
	static ScreenQuad screenQuad;
	static Shader* raytracingShader;
	static GLuint ssbo;
};