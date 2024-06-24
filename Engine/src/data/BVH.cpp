#include "data/BVH.h"

#include "component/Transform.h"
#include "data/mesh/Mesh.h"
#include "data/physics/HitInfo.h"
#include "data/physics/Ray.h"
#include "physics/RayIntersection.h"

int BVH::VISUAL_MAX_DEPTH = 0;

#pragma region Public Methods

BVH::BVH()
{
	hierarchy = std::make_shared<Node>();
}

BVH::BVH(const std::vector<Mesh>& meshes)
{
	hierarchy = std::make_shared<Node>();

	Update(meshes);
}

void BVH::Update(const std::vector<Mesh>& meshes)
{
	std::vector<Triangle> triangles;

	for (const Mesh& mesh : meshes)
	{
		hierarchy->Bounds.InsertMesh(mesh);
		std::vector<Triangle> meshTriangles = mesh.GetTriangles();
		triangles.insert(triangles.end(), meshTriangles.begin(), meshTriangles.end());
	}

	hierarchy->Triangles = triangles;
	split(hierarchy, 1);
}

void BVH::DrawNodes(const Transform& transform) const
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.z), glm::vec3(.0f, 0.0f, 1.0f))
							 * glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
							 * glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, .0f));

	drawNodes(transform, hierarchy, 0, rotationMatrix);
}

// we assume that ray is in bvh' local space
bool BVH::IntersectRay(const Ray& ray, HitInfo& outHitInfo) const
{
	return intersectRay(ray, hierarchy, outHitInfo);
}


int BVH::GetMaxDepth()
{
	return maxDepth;
}

#pragma endregion

#pragma region Private Methods

void BVH::split(std::shared_ptr<Node>& node, int depth)
{
	if (depth == maxDepth)
		return;

	int splitAxis = 0; float splitPos = 0; float cost = 0;
	chooseSplit(node, splitAxis, splitPos, cost);
	if (cost >= nodeCost(node->Bounds.GetSize(), static_cast<int>(node->Triangles.size())))
		return;

	node->Left = std::make_shared<Node>();
	node->Right = std::make_shared<Node>();

	glm::vec3 size = node->Bounds.GetSize();

	for (const Triangle& triangle : node->Triangles)
	{
		bool isInLeft = triangle.Center[splitAxis] < splitPos;
		std::shared_ptr<Node>& child = isInLeft ? node->Left : node->Right;
		child->Triangles.push_back(triangle);
		child->Bounds.InsertTriangle(triangle);
	}

	split(node->Left, depth + 1);
	split(node->Right, depth + 1);
}

void BVH::chooseSplit(const std::shared_ptr<Node>& node, int& outAxis, float& outPos, float& outCost) const
{
	constexpr int testPerAxisCount = 20;
	float bestCost = std::numeric_limits<float>::max();
	float bestPos = 0;
	int bestAxis = 0;

	for (int axis = 0; axis < 3; axis++)
	{
		float boundsStart = node->Bounds.Min[axis];
		float boundsEnd = node->Bounds.Max[axis];

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

float BVH::evaluateSplit(const std::shared_ptr<Node>& node, int& splitAxis, float& splitPos) const
{
	BoundingBox boundsA;
	BoundingBox boundsB;
	int inACount = 0;
	int inBCount = 0;

	for (const Triangle& triangle : node->Triangles)
	{
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

bool BVH::intersectRay(const Ray& ray, const std::shared_ptr<Node>& node, HitInfo& outHitInfo) const
{
	HitInfo boxHitInfo;
	RayAABoxIntersection(ray, node->Bounds, boxHitInfo);

	if (boxHitInfo.hit)
	{
		if (node->Left == nullptr && node->Right == nullptr)
		{
			HitInfo triangleHitInfo;
			for (const Triangle& triangle : node->Triangles)
			{
				RayTriangleIntersection(ray, triangle, triangleHitInfo);
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
			intersectRay(ray, node->Left, outHitInfo);
			intersectRay(ray, node->Right, outHitInfo);
		}
	}
	return outHitInfo.hit;
}

void BVH::drawNodes(const Transform& transform, const std::shared_ptr<Node>& node, int depth, const glm::mat4& rotationMatrix) const
{
	if (depth == maxDepth || depth == VISUAL_MAX_DEPTH || node == nullptr)
		return;

	if (node->Triangles.size() == 0)
		return;

	Color color = getColorForDepth(depth);

	if (depth == VISUAL_MAX_DEPTH - 1) 
		node->Bounds.Draw(transform, rotationMatrix, color);

	drawNodes(transform, node->Left, depth + 1, rotationMatrix);
	drawNodes(transform, node->Right, depth + 1, rotationMatrix);
}

Color BVH::getColorForDepth(int depth) const
{
	// 10 can be replaced by maxDepth if more depth color are needed
	float hue = (1.0f - static_cast<float>(depth) / 10) * 300.0f;
	return Color::HSLToRGB(hue / 360.0f, 1, 1);
}

#pragma endregion