#include "render/RayTracer.h"
#include "system/editor/Editor.h"

ScreenQuad RayTracer::screenQuad = {};
Shader* RayTracer::raytracingShader = nullptr;

#pragma region Static Methods

void RayTracer::Initialize(Shader* shader)
{
	raytracingShader = shader;

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

#pragma endregion