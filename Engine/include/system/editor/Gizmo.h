#pragma once

#include <map>

#include "data/mesh/WireCube.h"
#include "data/mesh/MeshData.h"
#include "component/Transform.h"
#include "data/Color.h"
#include "render/Shader.h"

class Gizmo
{
public:
	static void InitGizmos(Shader* shader);
	static void ReleaseGizmos();

	static void DrawWireCube(const Color& color = Color(), const Transform& transform = Transform());
	static void DrawWireCubeInstanced(const Color& color = Color(), const std::vector<glm::mat4>& transformMatrices = std::vector<glm::mat4>());

	static void DrawWireSphere(const Color& color = Color(), const Transform& transform = Transform());
	static void DrawWireCone(const Color& color = Color(), const Transform& transform = Transform());
	static void DrawArrow(const Color& color = Color(), Transform transform = Transform());
	static void DrawWireConeFrustum(const Color& color = Color(), const Transform& transform = Transform());

	static constexpr float GIZMO_WIDTH = 3;

private:
	static void bindShader(const Color& color, const Transform& transform);
	static void initCubeInstanceVBO();

	static Shader* shader;
	static std::map<GizmoType, std::unique_ptr<Mesh>> gizmos;
	
	// instance VBO
	static unsigned int cubeInstanceVBO;
};