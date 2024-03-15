#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraDirection
{
   FORWARD,
   BACKWARD,
   LEFT,
   RIGHT,
   UP,
   DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
   glm::vec3 Position;
   glm::vec3 Front;
   glm::vec3 Up;
   glm::vec3 Right;
   glm::vec3 WorldUp;

   // euler Angles
   float Yaw = -90.0f;
   float Pitch = 0.0f;

   // camera settings
   float MovementSpeed = 5.f;
   float MouseSensitivity = 0.1f;
   float Zoom = 45.0;

   // constructors
   Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
   Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

   // getters
   glm::mat4 GetViewMatrix();
   void SetSpeedFactor(float factor);

   // inputs processing
   void ProcessKeyboard(CameraDirection direction, float deltaTime);
   void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
   void ProcessMouseScroll(float yoffset);

private:
   float speedFactor = 1;

   // calculates the front vector from the Camera's (updated) Euler Angles
   void updateCameraVectors();
};