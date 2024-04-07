#include "system/Input.h"

#include <unordered_set>

namespace Input
{
	static GLFWwindow* window = nullptr;
	static std::unordered_set<int> pressedKeys = {};
	static std::unordered_set<int> pressedMouseButtons = {};

	void Initialize(GLFWwindow* _window)
	{
		window = _window;
	}

	// Returns true if the key was pressed this frame (not held)
	bool GetKeyDown(int key)
	{
		int state = glfwGetKey(window, key);
		if (state == GLFW_PRESS && pressedKeys.find(key) == pressedKeys.end())
		{
			pressedKeys.insert(key);
			return true;
		}
		else if (state == GLFW_RELEASE)
		{
			pressedKeys.erase(key);
		}
		return false;
	}

	// Returns true if the key is currently held down
	bool GetKey(int key)
	{
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	// Returns true if the key was released this frame
	bool GetKeyUp(int key)
	{
		return glfwGetKey(window, key) == GLFW_RELEASE;
	}

	bool GetMouseButtonDown(int button)
	{
		int state = glfwGetMouseButton(window, button);
		if (state == GLFW_PRESS && pressedMouseButtons.find(button) == pressedMouseButtons.end())
		{
			pressedMouseButtons.insert(button);
			return true;
		}
		else if (state == GLFW_RELEASE)
		{
			pressedMouseButtons.erase(button);
		}
		return false;
	}

	bool GetMouseButton(int button)
	{
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	bool GetMouseButtonUp(int button)
	{
		return glfwGetMouseButton(window, button) == GLFW_RELEASE;
	}

	glm::vec2 GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return glm::vec2(xpos, ypos);
	}
}