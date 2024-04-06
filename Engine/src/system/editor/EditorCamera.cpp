#include "system/editor/EditorCamera.h"

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

void EditorCamera::ProcessKeyboard(CameraDirection direction, float deltaTime)
{
   float velocity = MovementSpeed * speedFactor * deltaTime;
   glm::vec3 targetPosition = Position;

   switch (direction)
   {
      case FORWARD :
         targetPosition += Front * velocity;
         break;
      case BACKWARD :
         targetPosition -= Front * velocity;
         break;
      case LEFT :
         targetPosition -= Right * velocity;
         break;
      case RIGHT :
         targetPosition += Right * velocity;
         break;
      case UP :
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
   float deltaTime = Time::Get()->DeltaTime;

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

#pragma region Utility

const glm::mat4& EditorCamera::GetViewMatrix()
{
   return glm::lookAt(Position, Position + Front, Up);
}

const glm::mat4& EditorCamera::GetProjectionMatrix(float width, float height)
{
   return glm::perspective(glm::radians(Zoom), width / height, 0.1f, 1000.0f);
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
   glm::vec3 front;
   front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
   front.y = sin(glm::radians(Pitch));
   front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
   Front = glm::normalize(front);
   // also re-calculate the Right and Up vector
   Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
   Up = glm::normalize(glm::cross(Right, Front));
}

#pragma endregion