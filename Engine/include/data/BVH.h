#pragma once

#include <memory>
#include <vector>
#include <limits>

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

#include "data/BoundingBox.h"
#include "data/Color.h"

class Mesh;
struct HitInfo;
struct Ray;
struct Triangle;

struct BVHNode
{
	BoundingBox Bounds = BoundingBox(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	int TriangleIndex = 0;
	int TriangleCount = 0;

	int ChildIndex = 0;
};

class BVH
{
public:
	BVH();
	BVH(const std::vector<Mesh>& meshes);
	BVH(const BVH& other);

	const std::vector<Triangle>& GetTriangles() const;
	const std::vector<std::shared_ptr<BVHNode>>& GetNodes() const;

	void BuildBVH(const std::vector<Mesh>& meshes);

	void DrawNodes(const Transform& transform) const;
	// we assume that ray is in bvh' local space
	bool IntersectRay(const Ray& ray, HitInfo& outHitInfo) const;

	static int GetMaxDepth();
	static int VISUAL_MAX_DEPTH;

private:
	static constexpr int maxDepth = 15;

	std::vector<Triangle>  allTriangles;
	std::vector<std::shared_ptr<BVHNode>> allNodes;
	std::shared_ptr<BVHNode> hierarchy;
	
	void split(std::shared_ptr<BVHNode>& node, int depth = 0);
	void chooseSplit(const std::shared_ptr<BVHNode>& node, int& outAxis, float& outPos, float& outCost) const;
	float evaluateSplit(const std::shared_ptr<BVHNode>& node, int& splitAxis, float& splitPos) const;
	float nodeCost(const glm::vec3& size, int trianglesCount) const;
	bool intersectRay(const Ray& ray, const std::shared_ptr<BVHNode>& node, HitInfo& outHitInfo) const;

	// visualisation
	void drawNodes(const Transform& transform, const std::shared_ptr<BVHNode>& node, int depth, const glm::mat4& rotationMatrix) const;
	Color getColorForDepth(int depth) const;
};
