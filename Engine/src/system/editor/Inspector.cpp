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

	ImGui::Text(std::string("Name : " + entity->Name).c_str());
	ImGui::Separator();

	// inspect transform
	inspectTransform();

	// inspect components
	for (const Component* c : entity->GetComponents())
	{
		if (typeid(*c) == typeid(Model))
		{
			inspectModel(static_cast<const Model*>(c));
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

void Inspector::inspectModel(const Model* model) const
{
	ImGui::Text("Model");
}

#pragma endregion