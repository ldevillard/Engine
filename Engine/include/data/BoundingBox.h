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

	glm::vec3 GetSize() const;

	void InsertMesh(const Mesh& mesh);
	void InsertTriangle(const Triangle& triangle);
	void InsertPoint(const glm::vec3& point);

	void Draw(const Transform& transform, const Color& color = Color::Green) const;
	void Draw(const Transform& transform, glm::mat4 rotationMatrix , const Color& color = Color::Green) const;
};