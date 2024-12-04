#include "system/editor/EditorCamera.h"

#include "system/editor/ScreenSettings.h"
#include "system/Time.h"

#pragma region Public Methods

EditorCamera::EditorCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f))
	, MovementSpeed(SPEED)
	, Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

EditorCamera::EditorCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f))
	, MovementSpeed(SPEED)
	, Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

void EditorCamera::ProcessMatrices()
{
	viewMatrix = glm::lookAt(Position, Position + Front, Up);
	projectionMatrices[0] = glm::perspective(glm::radians(Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), Near, Far);
	projectionMatrices[1] = glm::perspective(glm::radians(Zoom), static_cast<float>(SCENE_WIDTH) / static_cast<float>(SCENE_HEIGHT), Near, Far);
	projectionMatrices[2] = glm::perspective(glm::radians(Zoom), static_cast<float>(RAYTRACED_SCENE_WIDTH) / static_cast<float>(RAYTRACED_SCENE_HEIGHT), Near, Far);
}

void EditorCamera::ProcessKeyboard(CameraDirection direction, float deltaTime)
{
	float velocity = MovementSpeed * speedFactor * deltaTime;
	glm::vec3 targetPosition = Position;

	switch (direction)
	{
	case FORWARD:
		targetPosition += Front * velocity;
		break;
	case BACKWARD:
		targetPosition -= Front * velocity;
		break;
	case LEFT:
		targetPosition -= Right * velocity;
		break;
	case RIGHT:
		targetPosition += Right * velocity;
		break;
	case UP:
		targetPosition += Up * velocity;
		break;
	case DOWN:
		targetPosition -= Up * velocity;
		break;
	}

	Position = targetPosition;
}

void EditorCamera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	float deltaTime = Time::DeltaTime;

	Yaw = std::lerp(Yaw, Yaw + xoffset, deltaTime * RotateSpeed);
	Pitch = std::lerp(Pitch, Pitch + yoffset, deltaTime * RotateSpeed);

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void EditorCamera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

void EditorCamera::SetPositionAndDirection(glm::vec3 position, glm::vec3 direction)
{
	Position = position;

	glm::vec3 normalizedDirection = glm::normalize(direction);

	// Yaw : angle around Y axis
	// Pitch : angle around X axis
	Yaw = glm::degrees(atan2(normalizedDirection.z, normalizedDirection.x));
	// arc sin for elevation
	Pitch = glm::degrees(asin(normalizedDirection.y)); 
	
	Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front.y = sin(glm::radians(Pitch));
	Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

#pragma region Utility

const glm::mat4& EditorCamera::GetViewMatrix() const
{
	return viewMatrix;
}

const glm::mat4& EditorCamera::GetProjectionMatrix(CameraProjectionType projectionType) const
{
	return projectionMatrices[projectionType];
}

void EditorCamera::SetSpeedFactor(float factor)
{
	speedFactor = factor;
}

#pragma endregion

#pragma endregion

#pragma region Private Methods

void EditorCamera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front = {};
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	glm::vec3 lerpFront = {};
	lerpFront.x = std::lerp(Front.x, front.x, Time::DeltaTime * RotateSpeed);
	lerpFront.y = std::lerp(Front.y, front.y, Time::DeltaTime * RotateSpeed);
	lerpFront.z = std::lerp(Front.z, front.z, Time::DeltaTime * RotateSpeed);

	Front = glm::normalize(lerpFront);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}

#pragma endregion