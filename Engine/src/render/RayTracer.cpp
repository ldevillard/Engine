#include "render/Raytracer.h"

#include "component/Transform.h"
#include "data/BVH.h"
#include "data/CubeMap.h"
#include "data/Triangle.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"

#pragma region Singleton Methods

// singleton override
void Raytracer::initialize()
{
	Singleton<Raytracer>::initialize();

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

	glGenBuffers(1, &textureSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, textureSSBO);

	setupScreenQuad();
}

#pragma endregion

#pragma region Public Methods

void Raytracer::Initialize(Shader* shader, ComputeShader* accumulate)
{
	Get();

	instance->raytracingShader = shader;
	instance->accumulateShader = accumulate;

	instance->initialize();
}

void Raytracer::Draw(const CubeMap& cubeMap)
{
	Editor::Get().GetRaytracingBuffer()->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	raytracingShader->Use();

	raytracingShader->SetVec2("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
	raytracingShader->SetUInt("frameCount", frameCount);

	const EditorCamera* camera = Editor::Get().GetCamera();

	const glm::mat4& projection = Editor::Get().GetCamera()->GetProjectionMatrix(CameraProjectionType::RAYTRACED_SCENE);
	const glm::mat4& view = Editor::Get().GetCamera()->GetViewMatrix();
	raytracingShader->SetMat4("invProjection", glm::inverse(projection));
	raytracingShader->SetMat4("invView", glm::inverse(view));
	raytracingShader->SetVec3("cameraPosition", camera->Position);
	raytracingShader->SetVec3("cameraRight", camera->Right);
	raytracingShader->SetVec3("cameraUp", camera->Up);

	// convert scene data to raytracing data (sphere at this moment)
	const std::vector<Model*> models = EntityManager::Get().GetModels();
	std::vector<RaytracingSphere> spheres = {};
	std::vector<RaytracingTriangle> triangles = {};
	std::vector<RaytracingCube> cubes = {};
	std::vector<RaytracingMesh> meshes = {};
	std::vector<RaytracingBVHNode> nodes = {};
	std::vector<GLuint64> handles = {};
	getSceneData(models, spheres, cubes, triangles, meshes, nodes, handles);

	const EditorSettings& settings = Editor::Get().GetSettings();

	raytracingShader->SetInt("skybox", 0);
	raytracingShader->SetVec3("skyboxColor", cubeMap.GetSkyboxLightColor());
	raytracingShader->SetUInt("skyboxEnabled", settings.Skybox);

	raytracingShader->SetInt("sphereCount", static_cast<int>(spheres.size()));
	raytracingShader->SetInt("cubeCount", static_cast<int>(cubes.size()));
	raytracingShader->SetInt("meshCount", static_cast<int>(meshes.size()));
	raytracingShader->SetInt("maxBounceCount", settings.MaxBounces);
	raytracingShader->SetInt("numberRaysPerPixel", settings.RaysPerPixel);
	raytracingShader->SetFloat("divergeStrength", settings.DivergeStrength);
	raytracingShader->SetUInt("bvhEnabled", settings.BVH == true ? 1u : 0u);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.ID);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sphereSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(RaytracingSphere), spheres.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cubeSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, cubes.size() * sizeof(RaytracingCube), cubes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(RaytracingTriangle), triangles.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshes.size() * sizeof(RaytracingMesh), meshes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, nodes.size() * sizeof(RaytracingBVHNode), nodes.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, textureSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, handles.size() * sizeof(GLuint64), handles.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(screenQuad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

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

void Raytracer::ResetFrameCount()
{
	frameCount = 0;
}

unsigned int Raytracer::GetFrameCount()
{
	return frameCount;
}

#pragma endregion

#pragma region Private Methods

void Raytracer::setupScreenQuad()
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

void Raytracer::getSceneData(const std::vector<Model*>& models, std::vector<RaytracingSphere>& inout_spheres, std::vector<RaytracingCube>& inout_cubes,
							 std::vector<RaytracingTriangle>& inout_triangles, std::vector<RaytracingMesh>& inout_meshes,
							 std::vector<RaytracingBVHNode>& inout_nodes, std::vector<GLuint64>& inout_handles)
{
	// this is to safely rebuild gpu data if there is models changements
	// like if scene is changed we don't want to keep to data of the previous scene
	if (meshCount > 0 && meshCount != models.size())
	{
		meshesTriangles.clear();
		meshesNodes.clear();
	}

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
		material.Textured = 0;

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
			// mesh part 
			const glm::mat4& transformMatrix = model->transform->GetTransformMatrix();
			RaytracingMesh raytracingMesh = {};
			raytracingMesh.FirstTriangleIndex = static_cast<int>(inout_triangles.size());
			raytracingMesh.FirstNodeIndex = static_cast<int>(inout_nodes.size());
			raytracingMesh.TransformMatrix = transformMatrix;
			raytracingMesh.InverseTransformMatrix = glm::inverse(transformMatrix);
			raytracingMesh.Material = material;

			// texture part
			// TODO: need to handle meshes that don't have textures
			const Mesh& mesh = model->GetMeshes()[0];
			if (mesh.Textures.size() > 0) 
			{
				raytracingMesh.Material.Textured = 1;
				inout_handles.push_back(mesh.Textures[0].TextureHandle);
			}
			else 
			{
				raytracingMesh.Material.Textured = 0;
				inout_handles.push_back(0);
			}


			// movement detection part, rebuild the raytracing data if there is modification
			const std::string& modelName = model->entity->Name;
			std::vector<RaytracingTriangle>& meshTriangles = meshesTriangles[modelName];
			std::vector<RaytracingBVHNode>& meshNodes = meshesNodes[modelName];

			if (meshTriangles.size() > 0 && meshesNodes.size() > 0)
			{
				inout_triangles.insert(inout_triangles.end(), meshTriangles.begin(), meshTriangles.end());
				inout_nodes.insert(inout_nodes.end(), meshNodes.begin(), meshNodes.end());
				inout_meshes.push_back(raytracingMesh);
				continue;
			}

			// triangles part
			const std::vector<Triangle>& allTriangles = model->GetBVH().GetTriangles();

			meshTriangles.reserve(allTriangles.size());
			for (size_t i = 0; i < allTriangles.size(); i++)
			{
				glm::vec3 A = allTriangles[i].A.Position;
				glm::vec3 B = allTriangles[i].B.Position;
				glm::vec3 C = allTriangles[i].C.Position;
			
				glm::vec3 normalA = allTriangles[i].A.Normal;
				glm::vec3 normalB = allTriangles[i].B.Normal;
				glm::vec3 normalC = allTriangles[i].C.Normal;

				glm::vec2 uvA = allTriangles[i].A.UV;
				glm::vec2 uvB = allTriangles[i].B.UV;
				glm::vec2 uvC = allTriangles[i].C.UV;
			
				RaytracingTriangle triangle = { A, B, C, normalA, normalB, normalC, uvA, uvB, uvC };
				meshTriangles.push_back(triangle);
			}

			// bvh part
			const std::vector<std::shared_ptr<BVHNode>>& allNodes = model->GetBVH().GetNodes();

			meshNodes.reserve(allNodes.size());
			for (size_t i = 0; i < allNodes.size(); i++)
			{
				RaytracingBVHNode node = {};
			
				node.BoundsMin = allNodes[i]->Bounds.Min;
				node.BoundsMax = allNodes[i]->Bounds.Max;
				node.TriangleIndex = allNodes[i]->TriangleIndex;
				node.TriangleCount = allNodes[i]->TriangleCount;
				node.ChildIndex = allNodes[i]->ChildIndex;
				meshNodes.push_back(node);
			}
		
			inout_triangles.insert(inout_triangles.end(), meshTriangles.begin(), meshTriangles.end());
			inout_nodes.insert(inout_nodes.end(), meshNodes.begin(), meshNodes.end());
			inout_meshes.push_back(raytracingMesh);
		}
	}
	meshCount = static_cast<int>(models.size());
}

#pragma endregion