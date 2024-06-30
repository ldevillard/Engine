#include "render/RayTracer.h"

#include "component/Transform.h"
#include "data/Triangle.h"
#include "data/BVH.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"

ScreenQuad RayTracer::screenQuad = {};
Shader* RayTracer::raytracingShader = nullptr;
ComputeShader* RayTracer::accumulateShader = nullptr;
GLuint RayTracer::sphereSSBO = 0;
GLuint RayTracer::cubeSSBO = 0;
GLuint RayTracer::triangleSSBO = 0;
GLuint RayTracer::meshSSBO = 0;
GLuint RayTracer::bvhSSBO = 0;
unsigned int RayTracer::frameCount = 0;
bool RayTracer::accumulate = false;

#pragma region Static Methods

void RayTracer::Initialize(Shader* shader, ComputeShader* accumulate)
{
	raytracingShader = shader;
	accumulateShader = accumulate;

	// initialize ssbo
	glGenBuffers(1, &sphereSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sphereSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sphereSSBO);

	glGenBuffers(1, &cubeSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cubeSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cubeSSBO);

	glGenBuffers(1, &triangleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, triangleSSBO);
	
	glGenBuffers(1, &meshSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, meshSSBO);

	glGenBuffers(1, &bvhSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, bvhSSBO);

	setupScreenQuad();
}

void RayTracer::Draw()
{
	Editor::Get().GetRaytracingBuffer()->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	raytracingShader->Use();

	raytracingShader->SetVec2("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
	raytracingShader->SetUInt("frameCount", frameCount);

	glm::mat4 projection = Editor::Get().GetCamera()->GetProjectionMatrix(static_cast<float>(RAYTRACED_SCENE_WIDTH), static_cast<float>(RAYTRACED_SCENE_HEIGHT));
	glm::mat4 view = Editor::Get().GetCamera()->GetViewMatrix();
	raytracingShader->SetMat4("invProjection", glm::inverse(projection));
	raytracingShader->SetMat4("invView", glm::inverse(view));
	raytracingShader->SetVec3("cameraPosition", Editor::Get().GetCamera()->Position);
	raytracingShader->SetVec3("cameraRight", Editor::Get().GetCamera()->Right);
	raytracingShader->SetVec3("cameraUp", Editor::Get().GetCamera()->Up);

	// convert scene data to raytracing data (sphere at this moment)
	const std::vector<Model*> models = EntityManager::Get().GetModels();
	std::vector<RaytracingSphere> spheres = {};
	std::vector<RayTracingTriangle> triangles = {};
	std::vector<RaytracingCube> cubes = {};
	std::vector<RayTracingMesh> meshes = {};
	std::vector<RayTracingBVHNode> nodes = {};
	getSceneData(models, spheres, cubes, triangles, meshes, nodes);

	raytracingShader->SetInt("sphereCount", static_cast<int>(spheres.size()));
	raytracingShader->SetInt("cubeCount", static_cast<int>(cubes.size()));
	raytracingShader->SetInt("meshCount", static_cast<int>(meshes.size()));
	raytracingShader->SetInt("maxBounceCount", Editor::Get().GetSettings().MaxBounces);
	raytracingShader->SetInt("numberRaysPerPixel", Editor::Get().GetSettings().RaysPerPixel);
	raytracingShader->SetFloat("divergeStrength", Editor::Get().GetSettings().DivergeStrength);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sphereSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(RaytracingSphere), spheres.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cubeSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cubes.size() * sizeof(RaytracingCube), cubes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(RayTracingTriangle), triangles.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshes.size() * sizeof(RayTracingMesh), meshes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, nodes.size() * sizeof(RayTracingBVHNode), nodes.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(screenQuad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	Editor::Get().GetRaytracingBuffer()->Unbind();
	Editor::Get().GetRaytracingBuffer()->Blit();

	// accumulate rays 
	if (Editor::Get().GetSettings().Accumulate)
	{
		if (!accumulate)
		{
			accumulate = true;
			ResetFrameCount();
		}

		accumulateShader->Use();
		accumulateShader->SetWorkSize(glm::uvec2(SCR_WIDTH, SCR_HEIGHT));
		accumulateShader->SetUInt("frameCount", frameCount);
		accumulateShader->SetTextures(Editor::Get().GetAccumulationBuffer()->GetFrameTexture()
			, Editor::Get().GetRaytracingBuffer()->GetFrameTexture());

		accumulateShader->Dispatch(glm::uvec2(8, 4));
		accumulateShader->Wait();
		frameCount++;
	}
	else
	{
		accumulate = false;
	}
}

void RayTracer::ResetFrameCount()
{
	frameCount = 0;
}

unsigned int RayTracer::GetFrameCount()
{
	return frameCount;
}

#pragma endregion

#pragma region Private Methods

void RayTracer::setupScreenQuad()
{
	screenQuad = ScreenQuad();

	glGenVertexArrays(1, &screenQuad.VAO);
	glGenBuffers(1, &screenQuad.VBO);
	glBindVertexArray(screenQuad.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuad.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad.vertices), &screenQuad.vertices, GL_STATIC_DRAW);
	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	// texture coord attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
}

void RayTracer::getSceneData(const std::vector<Model*>& models, std::vector<RaytracingSphere>& inout_spheres, std::vector<RaytracingCube>& inout_cubes,
							 std::vector<RayTracingTriangle>& inout_triangles, std::vector<RayTracingMesh>& inout_meshes,
							 std::vector<RayTracingBVHNode>& inout_nodes)
{
	for (Model* model : models)
	{
		// material setup
		Material mat = model->GetMaterial();
		RaytracingMaterial material = {};
		material.Color = mat.Diffuse;
		material.SpecularColor = mat.Specular;
		material.Flag = mat.Flag;
		material.Smoothness = std::clamp(mat.Smoothness, 0.f, 1.f);
		material.SpecularProbability = mat.SpecularProbability;
		material.EmissiveColor = mat.Emissive ? mat.Diffuse : glm::vec3(0.0f);
		material.EmissiveStrength = mat.Emissive ? mat.EmissiveStrength : 0.0f;

		if (model->ModelType == PrimitiveType::SpherePrimitive)
		{
			RaytracingSphere raytracingSphere = {};
			Sphere sphere = model->transform->AsSphere();

			raytracingSphere.Position = sphere.Position;
			raytracingSphere.Radius = sphere.Radius;
			raytracingSphere.Material = material;

			inout_spheres.push_back(raytracingSphere);
		}
		else if (model->ModelType == PrimitiveType::CubePrimitive)
		{
			RaytracingCube raytracingCube = {};
			const BoundingBox& obb = model->GetBoundingBox();

			raytracingCube.Min = obb.Min;
			raytracingCube.Max = obb.Max;
			raytracingCube.TransformMatrix = model->transform->GetTransformMatrix();
			raytracingCube.InverseTransformMatrix = glm::inverse(model->transform->GetTransformMatrix());
			raytracingCube.Material = material;

			inout_cubes.push_back(raytracingCube);
		}
		else
		{
			// Triangles part
			const std::vector<Triangle>& allTriangles = model->GetBVH().GetTriangles();
			std::vector<RayTracingTriangle> triangles = {};

			for (size_t i = 0; i < allTriangles.size(); i++)
			{
				glm::vec3 A = allTriangles[i].A.Position;
				glm::vec3 B = allTriangles[i].B.Position;
				glm::vec3 C = allTriangles[i].C.Position;
			
				glm::vec3 normalA = allTriangles[i].NormalA;
				glm::vec3 normalB = allTriangles[i].NormalB;
				glm::vec3 normalC = allTriangles[i].NormalC;
			
				RayTracingTriangle triangle = { A, B, C, normalA, normalB, normalC };
				triangles.push_back(triangle);
			}

			// BVH part
			const std::vector<std::shared_ptr<BVHNode>>& allNodes = model->GetBVH().GetNodes();
			std::vector<RayTracingBVHNode> nodes = {};

			for (size_t i = 0; i < allNodes.size(); i++)
			{
				RayTracingBVHNode node = {};
			
				node.BoundsMin = allNodes[i]->Bounds.Min;
				node.BoundsMax = allNodes[i]->Bounds.Max;
				node.TriangleIndex = allNodes[i]->TriangleIndex;
				node.TriangleCount = allNodes[i]->TriangleCount;
				node.ChildIndex = allNodes[i]->ChildIndex;
				nodes.push_back(node);
			}
		
			// Mesh part 
			RayTracingMesh raytracingMesh = {};
			raytracingMesh.FirstTriangleIndex = static_cast<int>(inout_triangles.size());
			raytracingMesh.FirstNodeIndex = static_cast<int>(inout_nodes.size());
			raytracingMesh.InverseTransformMatrix = glm::inverse(model->transform->GetTransformMatrix());
			raytracingMesh.Material = material;

			inout_triangles.insert(inout_triangles.end(), triangles.begin(), triangles.end());
			inout_nodes.insert(inout_nodes.end(), nodes.begin(), nodes.end());
			inout_meshes.push_back(raytracingMesh);
		}
	}
}

#pragma endregion