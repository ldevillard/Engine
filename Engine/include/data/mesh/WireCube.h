#pragma once

#include "Mesh.h"

class WireCube : public Mesh
{
public:
	WireCube();

	void Draw(Shader* shader) const override;
};