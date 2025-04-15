#pragma once

#include "utils/ImGui_Utils.h"
#include "system/entity/Entity.h"

// components
#include "component/Model.h"
#include "component/Light.h"
#include "component/physics/Fluid.h"

class Inspector
{
public:
	Inspector();
	~Inspector();

	void Inspect(const Entity* e);

private:
	void inspectTransform() const;
	void inspectModel(Model* model) const;
	void inspectLight(Light* light) const;
	void inspectFluid(Fluid* fluid) const;

	const Entity* entity = nullptr;
};