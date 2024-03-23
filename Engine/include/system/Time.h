#pragma once

class Time
{
public:
	// singleton
	static void CreateInstance();
	static void DestroyInstance();
	static Time* Get();
	
	void Update();
	const float GetFrameRate() const;

	float CurrentTime;
	float DeltaTime;

private:
	// singleton
	static Time* instance;

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
};