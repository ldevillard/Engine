#pragma once

#include "Mesh.h"

class WireConeFrustum : public Mesh
{
public:
	WireConeFrustum();

	void Draw(Shader* shader) const override;

private:
	void generateConeFrustum(unsigned int edgeCount, float topRadius, float bottomRadius, float height);
};