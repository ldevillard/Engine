#pragma once

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <glm/glm.hpp>

namespace ImGui_Utils
{
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 75.0f);
	void DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 75.0f);
	void SetPurpleTheme();
}