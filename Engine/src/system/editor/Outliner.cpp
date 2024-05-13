#include "system/editor/Outliner.h"

#include "system/editor/Editor.h"

// init static members but initialize them also in the Initialize 
// method because Color might not be initialized yet
Shader* Outliner::OutlineShader = nullptr;
Shader* Outliner::OutlineDilatingShader = nullptr;
ComputeShader* Outliner::OutlineBlitShader = nullptr;
Color Outliner::OutlineColor = glm::vec3(1.f, 0.565f, 0.161f);
float Outliner::OutlineWidth = 0.04f;

ScreenQuad Outliner::screenQuad = {};
float Outliner::radius = 2.5f;

#pragma region Static Methods

void Outliner::Initialize(Shader* outlineShader, Shader* outlineDilatingShader, ComputeShader* outlineBlitShader)
{
	OutlineShader = outlineShader;
	OutlineDilatingShader = outlineDilatingShader;
	OutlineBlitShader = outlineBlitShader;

	setupScreenQuad();
}

void Outliner::Setup()
{
	Editor::Get().GetOutlineBuffer(0)->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Outliner::OutlineShader->Use();

	// setup stencil buffer for outline
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
}

void Outliner::Reset()
{
	// reset stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);
}

void Outliner::Draw()
{
	// dillate outline
	Editor::Get().GetOutlineBuffer(1)->Bind();
	OutlineDilatingShader->Use();
	OutlineDilatingShader->SetFloat("radius", radius);
	OutlineDilatingShader->SetInt("screenTexture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Editor::Get().GetOutlineBuffer(0)->GetFrameTexture());
	glBindVertexArray(screenQuad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// bind to main frame buffer
	Editor::Get().GetSceneBuffer()->Bind();

	OutlineBlitShader->SetWorkSize(glm::uvec2(SCR_WIDTH, SCR_HEIGHT));

	OutlineBlitShader->Use();
	OutlineBlitShader->SetTextures(Editor::Get().GetSceneBuffer()->GetFrameTexture()
		, Editor::Get().GetOutlineBuffer(1)->GetFrameTexture());

	OutlineBlitShader->Dispatch(glm::uvec2(8, 4));
	OutlineBlitShader->Wait();
}

#pragma endregion

#pragma region Private Methods

void Outliner::setupScreenQuad()
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