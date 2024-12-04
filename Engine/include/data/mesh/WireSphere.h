#pragma once

#include "Mesh.h"

class WireSphere : public Mesh
{
public:
	WireSphere();

	void Draw(Shader* shader) const override;

private:
	void generateCircle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, int numVertices, const glm::vec3& axis);
};