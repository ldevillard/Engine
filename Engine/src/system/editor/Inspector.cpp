#include "system/editor/Inspector.h"

#include <algorithm>

#pragma region Public Methods

Inspector::Inspector()
{
}

Inspector::~Inspector()
{
}

void Inspector::Inspect(const Entity* e)
{
	entity = e;

	if (entity == nullptr)
	{
		throw std::runtime_error("FATAL: try to inspect a null entity!");
	}

	ImGui::Begin("Inspector");

	ImGui::Text(std::string("Entity : " + entity->Name).c_str());
	ImGui::Separator();

	// inspect transform
	inspectTransform();

	// inspect components
	for (Component* c : entity->GetComponents())
	{
		if (typeid(*c) == typeid(Model))
		{
			inspectModel(static_cast<Model*>(c));
		}
	}

	ImGui::End();
}

#pragma endregion

#pragma region Private Methods

void Inspector::inspectTransform() const
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Transform"))
	{
		ImGui_Utils::DrawVec3Control("Position", entity->transform->Position);
		ImGui_Utils::DrawVec3Control("Rotation", entity->transform->Rotation);
		ImGui_Utils::DrawVec3Control("Scale", entity->transform->Scale, 1.0f);
		ImGui::TreePop();
	}
}

int getMaterialIndex(const Material& material)
{
	auto it = std::find(Material::Names.begin(), Material::Names.end(), material.Name);
	if (it != Material::Names.end())
	{
		return std::distance(Material::Names.begin(), it);
	}
	else
	{
		return 0;
	}
}

void Inspector::inspectModel(Model* model) const
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Model"))
	{
		int currentItem = getMaterialIndex(model->GetMaterial());
		ImGui_Utils::DrawComboBoxControl("Material", currentItem, Material::Names);
		model->SetMaterialFromName(Material::Names[currentItem]);
		ImGui::TreePop();
	}	
}

#pragma endregion