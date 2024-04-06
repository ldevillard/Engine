#pragma once

#include <map>

#include "data/mesh/WireCube.h"
#include "data/mesh/WireSphere.h"
#include "data/mesh/MeshData.h"
#include "data/Transform.h"
#include "data/Color.h"

#include "render/Shader.h"

class Gizmo
{
public:
	static void InitGizmos(Shader* shader);

	static void DrawWireCube(const Color& color = Color(), const Transform& transform = Transform());
	static void DrawWireSphere(const Color& color = Color(), const Transform& transform = Transform());
	static void DrawWireCone(const Color& color = Color(), const Transform& transform = Transform());
	static void DrawArrow(const Color& color = Color(), Transform transform = Transform());
	static void DrawWireConeFrustum(const Color& color = Color(), const Transform& transform = Transform());

private:
	static void bindShader(const Color& color, const Transform& transform);

	static Shader* shader;
	static std::map<GizmoType, std::unique_ptr<Mesh>> gizmos;
};