#pragma once
#include <glm/glm.hpp>
class Camera
{

public:
    Camera();
    void ProcessInput();
    glm::vec3 GetCameraPos() const;

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float screenWidth, float screenHeight);
    enum CameraMode { FREECAM, SCENE_EDITOR, GAME };
    void ChangeMode(CameraMode mode);
    glm::vec3 GetRayDirection(float mouseX, float mouseY, float screenWidth, float screenHeight);
    void SetCameraPosition(glm::vec3 newPosition);
    void SetCameraRotation(float newPitch, float newYaw);

private:
    CameraMode cameraMode;

    glm::mat4 cameraViewMatrix;
    glm::mat4 cameraProjectionMatrix;

    glm::vec3 cameraPos;
    glm::vec3 directionVector;
    glm::vec3 upVector;
    glm::vec3 rightVector;

    // Camera parameters

    float yaw;	 // horizontal rotation XZ
    float pitch; // vertical rotation YZ
    const float fov = 90;
    const float cameraSpeed = 0.05f;
};
