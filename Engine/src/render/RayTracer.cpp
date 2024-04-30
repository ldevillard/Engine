#include "render/RayTracer.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"

ScreenQuad RayTracer::screenQuad = {};
Shader* RayTracer::raytracingShader = nullptr;
GLuint RayTracer::ssbo = 0;

#pragma region Static Methods

void RayTracer::Initialize(Shader* shader)
{
	raytracingShader = shader;

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

	glm::mat4 projection = Editor::Get()->GetCamera()->GetProjectionMatrix(static_cast<float>(RAYTRACED_SCENE_WIDTH), static_cast<float>(RAYTRACED_SCENE_HEIGHT));
	glm::mat4 view = Editor::Get()->GetCamera()->GetViewMatrix();
	raytracingShader->SetMat4("invProjection", glm::inverse(projection));
	raytracingShader->SetMat4("invView", glm::inverse(view));
	raytracingShader->SetVec3("cameraPosition", Editor::Get()->GetCamera()->Position);

	// convert scene data to raytracing data (sphere at this moment)
	const std::vector<Model*> models = EntityManager::Get()->GetModels();
	std::vector<RaytracingData> data = getSceneData(models);

	raytracingShader->SetInt("dataCount", static_cast<int>(data.size()));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(RaytracingData), data.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(screenQuad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	Editor::Get()->GetRaytracingBuffer()->Unbind();
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

std::vector<RaytracingData> RayTracer::getSceneData(const std::vector<Model*>& models)
{
	std::vector<RaytracingData> datas = {};

	for (Model* model : models)
	{
		if (model->ModelType == PrimitiveType::SpherePrimitive)
		{
			RaytracingData data = {};
			data.sphere = model->transform->AsSphere();
			data.color = model->GetMaterial().Diffuse;

			datas.push_back(data);
		}
	}

	return datas;
}

#pragma endregion