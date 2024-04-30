#include "system/editor/Inspector.h"
#include "system/editor/Editor.h"

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
		ImGui::Spacing();

		if (typeid(*c) == typeid(Model))
		{
			inspectModel(static_cast<Model*>(c));
		}
		if (typeid(*c) == typeid(Light))
		{
			inspectLight(static_cast<Light*>(c));
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
		return static_cast<int>(std::distance(Material::Names.begin(), it));
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
		
		// Ray Tracing
		{
			ImGui_Utils::DrawBoolControl("Emissive", model->GetMaterial().Emissive);
			ImGui_Utils::DrawFloatControl("Emissive Strength", model->GetMaterial().EmissiveStrength, 1.f);
		}

		ImGui::TreePop();
	}	
}

void Inspector::inspectLight(Light* light) const
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Light"))
	{
		int currentItem = static_cast<int>(light->lightType);

		ImGui_Utils::DrawComboBoxControl("Type", currentItem, Light::Names);
		ImGui_Utils::DrawColorControl("Color", light->color.Value);
		ImGui_Utils::DrawFloatControl("Intensity", light->Intensity, 1.f);
		light->SetLightTypeFromString(Light::Names[currentItem]);
		
		switch (light->lightType)
		{
			case Light::Point:
				ImGui_Utils::DrawFloatControl("Radius", light->Radius, 1.f);
				break;
			case Light::Spot:
				ImGui_Utils::DrawFloatControl("CutOff", light->CutOff, 1.f);
				ImGui_Utils::DrawFloatControl("OutCutOff", light->OutCutOff, 1.f);
				break;
		}
		
		ImGui::TreePop();
	}
}

#pragma endregion