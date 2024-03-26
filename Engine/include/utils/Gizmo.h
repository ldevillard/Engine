#pragma once

#include <map>

#include "data/mesh/WireCube.h"
#include "data/mesh/MeshData.h"
#include "data/Transform.h"
#include "data/Color.h"

#include "Shader.h"

class Gizmo
{
public:
	static void InitGizmos(Shader* shader);

	static void DrawWireCube(const Color& color = Color(), const Transform& transform = Transform());

private:
	static Shader* shader;
	static std::map<GizmoType, std::unique_ptr<Mesh>> gizmos;
};