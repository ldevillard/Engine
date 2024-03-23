#include "system/Time.h"

#include <glfw3.h>

// singleton instance
Time* Time::instance = nullptr;

#pragma region Public Methods

void Time::CreateInstance()
{
	if (instance == nullptr)
	{
		instance = new Time();
	}
}

void Time::DestroyInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

Time* Time::Get()
{
	return instance;
}

void Time::Update()
{
	CurrentTime = static_cast<float>(glfwGetTime());
	currentFrame = CurrentTime;
	DeltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

const float Time::GetFrameRate() const
{
	if (DeltaTime == 0.0f)
		return 0.0f;
	return 1.0f / DeltaTime;
}

#pragma endregion