#pragma once

#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <glm/glm.hpp>

namespace ImGui_Utils
{
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 75.0f);
	void DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 75.0f);
	void DrawBoolControl(const std::string& label, bool& value, float columnWidth = 75.0f);
	void DrawComboBoxControl(const std::string& label, int& selectedItem, const std::vector<const char*>& options, float columnWidth = 75.0f);
	void DrawColorControl(const std::string& label, glm::vec3& color, float columnWidth = 75.0f);

	void SetPurpleTheme();
}