#pragma once

namespace Time 
{	
	extern float CurrentTime;
	extern float DeltaTime;

	extern float lastFrame;
	extern float currentFrame;

	void Update();
	const float FrameRate();
}