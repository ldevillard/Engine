#pragma once

#include "Mesh.h"

class WireConeFrustum : public Mesh
{
public:
	WireConeFrustum();

	void Draw(Shader* shader) override;

private:
	void generateConeFrustum(unsigned int edgeCount, float topRadius, float bottomRadius, float height);
};