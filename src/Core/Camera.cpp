
#include <glm/glm.hpp>
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Input.h"
#include <iostream>

Camera::Camera()
{

    // Camera Initialization

    cameraMode = FREECAM;
    cameraPos = glm::vec3(0, 10.0f, 3.0f);
    directionVector = glm::vec3(0, 0, -1.0f);
    upVector = glm::vec3(0, 1.0f, 0);
    yaw = -90;
    pitch = 0.0f;
    std::cout << "[INIT] Camera instance successfully!" << std::endl;
}

glm::mat4 Camera::GetViewMatrix()
{

    cameraViewMatrix = glm::lookAt(cameraPos, cameraPos + directionVector, upVector);
    return this->cameraViewMatrix;
}
glm::mat4 Camera::GetProjectionMatrix(float screenWidth, float screenHeight)
{

    cameraProjectionMatrix =
	glm::perspective(glm::radians(this->fov), screenWidth / screenHeight, 0.1f, 100.0f);
    return this->cameraProjectionMatrix;
}

void Camera::ProcessInput()
{

    if (Input::IsKeyPressed(Input::F1_KEY))
	this->ChangeMode(FREECAM);
    if (Input::IsKeyPressed(Input::F2_KEY))
	this->ChangeMode(SCENE_EDITOR);
    float scroll = Input::GetMouseScroll();
    switch (cameraMode)
    {
    case (FREECAM): {
	float sensitivity = 0.2f;
	float xOffset = Input::GetMouseDeltaX() * sensitivity;
	float yOffset = Input::GetMouseDeltaY() * sensitivity;
	yaw += xOffset;
	pitch -= yOffset;

	if (pitch > 89.0f)
	    pitch = 89.f;
	if (pitch < -89.0f)
	    pitch = -89.f;

	// New Direction
	glm::vec3 newDirection;
	newDirection.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	newDirection.y = sin(glm::radians(pitch));
	newDirection.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	this->directionVector = glm::normalize(newDirection);

	// Keyboard movement
	if (Input::IsKeyDown(Input::W_KEY))
	{
	    cameraPos += (directionVector * cameraSpeed);
	}
	if (Input::IsKeyDown(Input::S_KEY))
	{
	    cameraPos -= (directionVector * cameraSpeed);
	}
	if (Input::IsKeyDown(Input::A_KEY))
	{
	    rightVector = glm::normalize(glm::cross(upVector, directionVector));
	    cameraPos += (rightVector * cameraSpeed);
	}
	if (Input::IsKeyDown(Input::D_KEY))
	{
	    rightVector = glm::normalize(glm::cross(upVector, directionVector));
	    cameraPos -= (rightVector * cameraSpeed);
	}
	break;
    }
    case (SCENE_EDITOR): {

	// Zoom
	if (scroll != 0.0f)
	{
	    float zoomSpeed = 2.0f;
	    cameraPos += directionVector * scroll * zoomSpeed;
	}
	// Keyboard movement
	if (Input::IsKeyDown(Input::W_KEY))
	{
	    cameraPos.z -= cameraSpeed;
	}
	if (Input::IsKeyDown(Input::S_KEY))
	{
	    cameraPos.z += cameraSpeed;
	}
	if (Input::IsKeyDown(Input::A_KEY))
	{

	    cameraPos += (rightVector * cameraSpeed);
	}
	if (Input::IsKeyDown(Input::D_KEY))
	{

	    cameraPos -= (rightVector * cameraSpeed);
	}
	break;
    }
    }
}

void Camera::ChangeMode(Camera::CameraMode mode)
{

    switch (mode)
    {
    case (FREECAM): {
	pitch = -0.0f;
	yaw = -90.0f;
	// fixed vectors
	directionVector.x = 0;
	directionVector.y = 0;
	directionVector.z = -1;

	upVector.x = 0;
	upVector.y = 1;
	upVector.z = 0;
	Input::SetRelativeMouse(true);
	this->cameraMode = FREECAM;

	break;
    }
    case (SCENE_EDITOR): {
	cameraPos.y = 10;
	Input::SetRelativeMouse(false);
	// fixed pitch
	pitch = -80.0f;
	yaw = -90.0f;
	// Fixed vectors
	// direction aka where the camera is looking
	directionVector.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	directionVector.y = sin(glm::radians(pitch));
	directionVector.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	directionVector = glm::normalize(directionVector);
	// upvector
	upVector.x = 0;
	upVector.y = 1;
	upVector.z = 0;

	rightVector = glm::normalize(glm::cross(upVector, directionVector));

	this->cameraMode = SCENE_EDITOR;
	break;
    }
    }
}

glm::vec3 Camera::GetRayDirection(float mouseX, float mouseY, float screenWidth, float screenHeight)
{
    float x = (mouseX / screenWidth) * 2.0f - 1.0f;
    float y = 1.0f - (mouseY / screenHeight) * 2.0f;

    glm::vec4 rayClip = glm::vec4(x, y, -1, 1);

    glm::vec4 rayEye = glm::inverse(cameraProjectionMatrix) * rayClip;
    rayEye.z = -1;
    rayEye.w = 0;
    glm::vec3 rayWorld = glm::vec3(glm::inverse(cameraViewMatrix) * rayEye);
    rayWorld = glm::normalize(rayWorld);
    return rayWorld;
}

glm::vec3 Camera::GetCameraPos() const
{
    return cameraPos;
}