#include "data/BVH.h"

#include "data/mesh/Mesh.h"

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
	split(hierarchy);
}

void BVH::DrawNodes(const Transform& transform, const Color& color) const
{
	drawNodes(transform, hierarchy, 0, color);
}

#pragma endregion

#pragma region Private Methods

void BVH::drawNodes(const Transform& transform, const std::shared_ptr<Node>& node, uint32_t depth, const Color& color) const
{
	if (depth == maxDepth || node == nullptr)
		return;

	if (node->Triangles.size() == 0)
		return;

	node->Bounds.Draw(transform, color);

	drawNodes(transform, node->Left, depth + 1, color);
	drawNodes(transform, node->Right, depth + 1, color);
}

void BVH::split(std::shared_ptr<Node>& node, uint32_t depth)
{
	if (depth == maxDepth)
		return;

	node->Left = std::make_shared<Node>();
	node->Right = std::make_shared<Node>();

	glm::vec3 size = node->Bounds.GetSize();
	int longestAxis = size.x > size.y ? (size.x > size.z ? 0 : 2) : 1;
	float splitValue = node->Bounds.Center[longestAxis];

	for (const Triangle& triangle : node->Triangles)
	{
		glm::vec3 triangleCenter = (triangle.A.Position + triangle.B.Position + triangle.C.Position) / 3.0f;

		bool isInLeft = triangleCenter[longestAxis] < splitValue;
		std::shared_ptr<Node>& child = isInLeft ? node->Left : node->Right;
		child->Triangles.push_back(triangle);
		child->Bounds.InsertTriangle(triangle);
	}

	split(node->Left, depth + 1);
	split(node->Right, depth + 1);
}

#pragma endregion