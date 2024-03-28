#pragma once

#include "Mesh.h"

class WireCone : public Mesh
{
public:
	WireCone();

	void Draw(Shader* shader) override;

private:
    void generateCone(unsigned int edgeCount);
};