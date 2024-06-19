#pragma once

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

#include "data/Color.h"

class Mesh;
class Transform;
struct Triangle;

class BoundingBox
{
public:
	glm::vec3 Min;
	glm::vec3 Max;
	glm::vec3 Center;

	BoundingBox();
	BoundingBox(const glm::vec3& min, const glm::vec3& max);

	void InsertTriangle(const Triangle& triangle);
	void InsertMesh(const Mesh& mesh);

	void Draw(const Transform& transform, const Color& color = Color::Green) const;

private:
	void insertPoint(const glm::vec3& point);
};