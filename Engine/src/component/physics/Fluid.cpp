#include "component/physics/Fluid.h"

#include "component/Model.h"
#include "system/editor/Gizmo.h"

#pragma region Public Methods

Fluid::Fluid() : Component()
{
	sphereMesh = Model::PrimitivesModels[PrimitiveType::SpherePrimitive]->GetMeshes()[0];
}

void Fluid::Compute()
{
	Gizmo::DrawWireCube(Color::Blue, *transform);
	
	// binding material data
	shader->Use();
	shader->SetVec3("material.ambient", material.Ambient);
	shader->SetVec3("material.diffuse", material.Diffuse);
	shader->SetVec3("material.specular", material.Specular);
	shader->SetFloat("material.shininess", material.Shininess);

	sphereMesh.Draw(shader);
}

Component* Fluid::Clone()
{
	Fluid* newFluid = new Fluid();
	return newFluid;
}

nlohmann::ordered_json Fluid::Serialize() const
{
	nlohmann::ordered_json json;

	json["type"] = "Fluid";

	return json;
}

void Fluid::Deserialize(const nlohmann::ordered_json& json)
{
	// Deserialize implementation
}

#pragma endregion