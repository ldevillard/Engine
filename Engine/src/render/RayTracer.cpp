#include "render/RayTracer.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"

ScreenQuad RayTracer::screenQuad = {};
Shader* RayTracer::raytracingShader = nullptr;
ComputeShader* RayTracer::accumulateShader = nullptr;
GLuint RayTracer::ssbo = 0;
unsigned int RayTracer::frameCount = 0;
bool RayTracer::accumulate = false;

#pragma region Static Methods

void RayTracer::Initialize(Shader* shader, ComputeShader* accumulate)
{
	raytracingShader = shader;
	accumulateShader = accumulate;

	// initialize ssbo
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

	setupScreenQuad();
}

void RayTracer::Draw()
{
	Editor::Get()->GetRaytracingBuffer()->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	raytracingShader->Use();

	raytracingShader->SetVec2("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
	raytracingShader->SetUInt("frameCount", frameCount);

	glm::mat4 projection = Editor::Get()->GetCamera()->GetProjectionMatrix(static_cast<float>(RAYTRACED_SCENE_WIDTH), static_cast<float>(RAYTRACED_SCENE_HEIGHT));
	glm::mat4 view = Editor::Get()->GetCamera()->GetViewMatrix();
	raytracingShader->SetMat4("invProjection", glm::inverse(projection));
	raytracingShader->SetMat4("invView", glm::inverse(view));
	raytracingShader->SetVec3("cameraPosition", Editor::Get()->GetCamera()->Position);

	// convert scene data to raytracing data (sphere at this moment)
	const std::vector<Model*> models = EntityManager::Get()->GetModels();
	std::vector<RaytracingSphere> spheres = getSceneData(models);

	raytracingShader->SetInt("dataCount", static_cast<int>(spheres.size()));
	raytracingShader->SetInt("maxBounceCount", Editor::Get()->GetSettings().MaxBounces);
	raytracingShader->SetInt("numberRaysPerPixel", Editor::Get()->GetSettings().RaysPerPixel);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(RaytracingSphere), spheres.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(screenQuad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	Editor::Get()->GetRaytracingBuffer()->Unbind();

	// accumulate rays 
	if (Editor::Get()->GetSettings().Accumulate)
	{
		if (!accumulate)
		{
			accumulate = true;
			ResetFrameCount();
		}

		accumulateShader->Use();
		accumulateShader->SetWorkSize(glm::uvec2(SCR_WIDTH, SCR_HEIGHT));
		accumulateShader->SetUInt("frameCount", frameCount);
		accumulateShader->SetTextures(Editor::Get()->GetAccumulationBuffer()->GetFrameTexture()
			, Editor::Get()->GetRaytracingBuffer()->GetFrameTexture());

		accumulateShader->Dispatch(glm::uvec2(8, 4));
		accumulateShader->Wait();
	}
	else
	{
		accumulate = false;
	}

	frameCount++;
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

std::vector<RaytracingSphere> RayTracer::getSceneData(const std::vector<Model*>& models)
{
	std::vector<RaytracingSphere> spheres = {};

	for (Model* model : models)
	{
		if (model->ModelType == PrimitiveType::SpherePrimitive)
		{
			RaytracingSphere raytracingSphere = {};
			Sphere sphere = model->transform->AsSphere();
			
			Material mat = model->GetMaterial();

			raytracingSphere.Position = sphere.Position;
			raytracingSphere.Radius = sphere.Radius;
			raytracingSphere.Material.Color = mat.Diffuse;
			raytracingSphere.Material.Smoothness = std::clamp(mat.Smoothness, 0.f, 1.f);

			if (mat.Emissive)
			{
				raytracingSphere.Material.EmissiveColor = mat.Diffuse;
				raytracingSphere.Material.EmissiveStrength = mat.EmissiveStrength;
			}
			else
			{
				raytracingSphere.Material.EmissiveColor = glm::vec3(0.0f);
				raytracingSphere.Material.EmissiveStrength = 0.0f;
			}

			spheres.push_back(raytracingSphere);
		}
	}

	return spheres;
}

#pragma endregion