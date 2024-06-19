#pragma once

#include <memory>
#include <vector>

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

#include "data/BoundingBox.h"
#include "data/Color.h"

class Mesh;
struct Triangle;

class BVH
{
public:
	BVH();
	BVH(const std::vector<Mesh>& meshes);

	void Update(const std::vector<Mesh>& meshes);

	void DrawNodes(const Transform& transform, const Color& color = Color::Cyan) const;

private:
	struct Node
	{
		BoundingBox Bounds;

		std::vector<Triangle> Triangles;

		std::shared_ptr<Node> Left;
		std::shared_ptr<Node> Right;
	};

	static constexpr uint32_t maxDepth = 10;

	std::shared_ptr<Node> hierarchy;
	
	void drawNodes(const Transform& transform, const std::shared_ptr<Node>& node, uint32_t depth, const Color& color) const;
	void split(std::shared_ptr<Node>& node, uint32_t depth = 0);

};
