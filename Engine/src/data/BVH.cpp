#include "data/BVH.h"

#include <algorithm>

#include "component/Transform.h"
#include "data/mesh/Mesh.h"
#include "data/physics/HitInfo.h"
#include "data/physics/Ray.h"
#include "physics/RayIntersection.h"
#include "system/editor/Gizmo.h"

int BVH::VISUAL_MAX_DEPTH = 0;

#pragma region Public Methods

BVH::BVH()
{
	hierarchy = std::make_shared<BVHNode>();
}

BVH::BVH(const std::vector<Mesh>& meshes)
{
	hierarchy = std::make_shared<BVHNode>();

	BuildBVH(meshes);
}

BVH::BVH(const BVH& other) : hierarchy(other.hierarchy), allNodes(other.allNodes), allTriangles(other.allTriangles)
{
}

const std::vector<Triangle>& BVH::GetTriangles() const
{
	return allTriangles;
}

const std::vector<std::shared_ptr<BVHNode>>& BVH::GetNodes() const
{
	return allNodes;
}

void BVH::BuildBVH(const std::vector<Mesh>& meshes)
{
	std::vector<Triangle> triangles;

	for (const Mesh& mesh : meshes)
	{
		hierarchy->Bounds.InsertMesh(mesh);
		std::vector<Triangle> meshTriangles = mesh.GetTriangles();
		triangles.insert(triangles.end(), meshTriangles.begin(), meshTriangles.end());
	}

	allTriangles = triangles;
	hierarchy->TriangleIndex = 0;
	hierarchy->TriangleCount = static_cast<int>(allTriangles.size());
	allNodes.push_back(hierarchy);

	split(hierarchy, 1);
}

void BVH::DrawNodes(const Transform& transform) const
{
	if (allNodes.size() == 0) return;

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.z), glm::vec3(.0f, 0.0f, 1.0f))
							 * glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
							 * glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, .0f));

	std::vector<glm::mat4> transformMatricesToDraw;

	drawNodes(transform, hierarchy, 0, rotationMatrix, transformMatricesToDraw);

	Gizmo::DrawWireCubeInstanced(getColorForDepth(VISUAL_MAX_DEPTH), transformMatricesToDraw);
}

// we assume that ray is in bvh' local space
bool BVH::IntersectRay(const Ray& ray, HitInfo& outHitInfo) const
{
	if (allTriangles.size() == 0) return false; 
	return intersectRay(ray, hierarchy, outHitInfo);
}

int BVH::GetMaxDepth()
{
	return maxDepth;
}

#pragma endregion

#pragma region Private Methods

void BVH::split(std::shared_ptr<BVHNode>& node, int depth)
{
	if (depth == maxDepth)
		return;

	int splitAxis = 0; float splitPos = 0; float cost = 0;
	chooseSplit(node, splitAxis, splitPos, cost);
	if (cost >= nodeCost(node->Bounds.GetSize(), node->TriangleCount))
		return;

	std::shared_ptr<BVHNode> leftChild = std::make_shared<BVHNode>();
	std::shared_ptr<BVHNode> rightChild = std::make_shared<BVHNode>();

	leftChild->TriangleIndex = node->TriangleIndex;
	rightChild->TriangleIndex = node->TriangleIndex;

	for (int i = node->TriangleIndex; i < node->TriangleIndex + node->TriangleCount; ++i)
	{
		bool isInLeft = allTriangles[i].Center[splitAxis] < splitPos;
		std::shared_ptr<BVHNode>& child = isInLeft ? leftChild : rightChild;
		child->Bounds.InsertTriangle(allTriangles[i]);
		child->TriangleCount++;

		if (isInLeft)
		{
			int swap = child->TriangleIndex + child->TriangleCount - 1;
			std::swap(allTriangles[i], allTriangles[swap]);
			rightChild->TriangleIndex++;
		}
	}

	node->ChildIndex = static_cast<int>(allNodes.size());
	allNodes.push_back(leftChild);
	allNodes.push_back(rightChild);

	split(leftChild, depth + 1);
	split(rightChild, depth + 1);
}

void BVH::chooseSplit(const std::shared_ptr<BVHNode>& node, int& outAxis, float& outPos, float& outCost) const
{
	constexpr int testPerAxisCount = 5;
	float bestCost = std::numeric_limits<float>::max();
	float bestPos = 0;
	int bestAxis = 0;

	for (int axis = 0; axis < 3; axis++)
	{
		float boundsStart = node->Bounds.Min[axis];
		float boundsEnd = node->Bounds.Max[axis];

		if (boundsStart == boundsEnd)
			continue;

		for (int i = 0; i < testPerAxisCount; i++)
		{
			float splitT = (i + 1) / (testPerAxisCount + 1.f);

			float pos = std::lerp(boundsStart, boundsEnd, splitT);
			float cost = evaluateSplit(node, axis, pos);

			if (cost < bestCost)
			{
				bestCost = cost;
				bestPos = pos;
				bestAxis = axis;
			}
		}
	}

	outAxis = bestAxis;
	outPos = bestPos;
	outCost = bestCost;
}

float BVH::evaluateSplit(const std::shared_ptr<BVHNode>& node, int& splitAxis, float& splitPos) const
{
	BoundingBox boundsA(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	BoundingBox boundsB(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	int inACount = 0;
	int inBCount = 0;

	for (int i = node->TriangleIndex; i < node->TriangleIndex + node->TriangleCount; ++i)
	{
		const Triangle& triangle = allTriangles[i];
		if (triangle.Center[splitAxis] < splitPos)
		{
			boundsA.InsertTriangle(triangle);
			inACount++;
		}
		else
		{
			boundsB.InsertTriangle(triangle);
			inBCount++;
		}
	}

	return nodeCost(boundsA.GetSize(), inACount) + nodeCost(boundsB.GetSize(), inBCount);
}

float BVH::nodeCost(const glm::vec3& size, int trianglesCount) const
{
	float halfArea = size.x * (size.y + size.z) + size.y * size.z;
	return halfArea * trianglesCount;
}

bool BVH::intersectRay(const Ray& ray, const std::shared_ptr<BVHNode>& node, HitInfo& outHitInfo) const
{
	HitInfo boxHitInfo;
	RayAABoxIntersection(ray, node->Bounds, boxHitInfo);

	if (boxHitInfo.hit)
	{
		if ((node->ChildIndex == 0 && node != hierarchy) || allTriangles.size() < 10)
		{
			HitInfo triangleHitInfo;
			for (int i = node->TriangleIndex; i < node->TriangleIndex + node->TriangleCount; ++i)
			{
				RayTriangleIntersection(ray, allTriangles[i], triangleHitInfo);
				if (triangleHitInfo.distance < outHitInfo.distance)
				{
					outHitInfo.hit = triangleHitInfo.hit;
					outHitInfo.hitPoint = triangleHitInfo.hitPoint;
					outHitInfo.distance = triangleHitInfo.distance;
				}
			}
		}
		else
		{
			intersectRay(ray, allNodes[node->ChildIndex], outHitInfo);
			intersectRay(ray, allNodes[node->ChildIndex + 1], outHitInfo);
		}
	}
	return outHitInfo.hit;
}

void BVH::drawNodes(const Transform& transform, const std::shared_ptr<BVHNode>& node, int depth,
	const glm::mat4& rotationMatrix, std::vector<glm::mat4>& outTransformMatrices) const
{
	if (depth == maxDepth || depth == VISUAL_MAX_DEPTH)
		return;

	if (node->TriangleCount > 0 && depth == VISUAL_MAX_DEPTH - 1)
	{
		// make a transform matrix from the bounding box and add it to the list
		glm::vec3 scale = transform.Scale * glm::abs(node->Bounds.Max - node->Bounds.Min) * 0.5f;
		glm::vec3 center = node->Bounds.GetCenter() * transform.Scale;
		center = glm::vec3(rotationMatrix * glm::vec4(center, 1.0f)) + transform.Position;
		
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), center);
		modelMatrix = modelMatrix * rotationMatrix;
		modelMatrix = glm::scale(modelMatrix, scale);

		outTransformMatrices.push_back(modelMatrix);
	}

	if ((node->ChildIndex > 0 || node == hierarchy) && allNodes.size() > 1)
	{
		drawNodes(transform, allNodes[node->ChildIndex + 1], depth + 1, rotationMatrix, outTransformMatrices);
		drawNodes(transform, allNodes[node->ChildIndex], depth + 1, rotationMatrix, outTransformMatrices);
	}
}

Color BVH::getColorForDepth(int depth) const
{
	// 10 can be replaced by maxDepth if more depth color are needed
	float hue = (1.0f - static_cast<float>(depth) / 10) * 300.0f;
	return Color::HSLToRGB(hue / 360.0f, 1, 1);
}

#pragma endregion