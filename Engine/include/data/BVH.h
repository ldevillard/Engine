#pragma once

#include <memory>
#include <vector>

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

#include "data/BoundingBox.h"
#include "data/Color.h"

class Mesh;
struct HitInfo;
struct Ray;
struct Triangle;

class BVH
{
public:
	BVH();
	BVH(const std::vector<Mesh>& meshes);

	void Update(const std::vector<Mesh>& meshes);

	void DrawNodes(const Transform& transform, const Color& color = Color::Cyan) const;
	// we assume that ray is in bvh' local space
	bool IntersectRay(const Ray& ray, HitInfo& outHitInfo) const;

	static int GetMaxDepth();
	static int VISUAL_MAX_DEPTH;

private:
	struct Node
	{
		BoundingBox Bounds;

		std::vector<Triangle> Triangles;

		std::shared_ptr<Node> Left;
		std::shared_ptr<Node> Right;
	};

	static constexpr int maxDepth = 10;

	std::shared_ptr<Node> hierarchy;
	
	void split(std::shared_ptr<Node>& node, int depth = 0);
	void drawNodes(const Transform& transform, const std::shared_ptr<Node>& node, int depth, const glm::mat4& rotationMatrix, const Color& color) const;
	bool intersectRay(const Ray& ray, const std::shared_ptr<Node>& node, HitInfo& outHitInfo) const;

};
