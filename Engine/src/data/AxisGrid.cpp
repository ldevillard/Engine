#include "data/AxisGrid.h"

#include "render/Shader.h"

#pragma region Public Methods

AxisGrid::AxisGrid(Shader* sh) : shader(sh)
{
}

AxisGrid::~AxisGrid()
{
	glDeleteVertexArrays(1, &screenQuad.VAO);
	glDeleteBuffers(1, &screenQuad.VBO);
}

void AxisGrid::Draw(const glm::mat4& view, const glm::mat4& projection) const
{
	shader->Use();

	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection);

	glBindVertexArray(screenQuad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

#pragma endregion

#pragma region Private Methods

void AxisGrid::setupScreenQuad()
{
	screenQuad = ScreenQuad();

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