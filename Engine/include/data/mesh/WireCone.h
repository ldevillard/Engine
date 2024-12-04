#pragma once

#include "Mesh.h"

class WireCone : public Mesh
{
public:
	WireCone();

	void Draw(Shader* shader) const override;

private:
    void generateCone(unsigned int edgeCount);
};