#pragma once

#include "utils/ImGui_Utils.h"
#include "Entity.h"

// components
#include "component/Model.h"

class Inspector
{
public:
	Inspector();
	~Inspector();

	void Inspect(const Entity* e);

private:
	void inspectTransform() const;
	void inspectModel(const Model* model) const;

	const Entity* entity = nullptr;
};