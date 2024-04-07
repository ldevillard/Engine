#include "system/Time.h"

#include <glfw3.h>

namespace Time
{
	float CurrentTime = 0.0f;
	float DeltaTime = 0.0f;

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;

	void Update()
	{
		CurrentTime = static_cast<float>(glfwGetTime());
		currentFrame = CurrentTime;
		DeltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
	
	const float FrameRate()
	{
		if (DeltaTime == 0.0f)
			return 0.0f;
		return 1.0f / DeltaTime;
	}
}