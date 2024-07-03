#pragma once

#include "data/mesh/MeshData.h"
#include "maths/Math.h"

class Shader;

class AxisGrid
{
public:
	AxisGrid(Shader* sh);
	~AxisGrid();

	void Draw(const glm::mat4& view, const glm::mat4& projection) const;

private:
	void setupScreenQuad();

	Shader* shader;
	ScreenQuad screenQuad;
};