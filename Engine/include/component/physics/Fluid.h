#pragma once

#include "component/Component.h"
#include "data/mesh/Mesh.h"
#include "data/Material.h"

class Fluid : public Component
{
public:
	Fluid();

	void Compute() override;
	Component* Clone() override;
	
	// serialization
	nlohmann::ordered_json Serialize() const override;
	void Deserialize(const nlohmann::ordered_json& json) override;

private:
	// render data
	Mesh sphereMesh;
	Material material = Material::Sapphire;
};

REGISTER_COMPONENT_TYPE(Fluid);