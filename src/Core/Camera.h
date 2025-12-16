#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include "Input.h"

class Camera{

    public:
    Camera();
    void ProcessInput();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float screenWidth, float screenHeight);

    private:
    glm::mat4 cameraViewMatrix;
    glm::mat4 cameraProjectionMatrix;

    glm::vec3 cameraPos;
    glm::vec3 directionVector;
    glm::vec3 upVector;
    glm::vec3 rightVector;

    //Camera parameters
    
    float yaw; //horizontal rotation XZ
    float pitch; //vertical rotation YZ
    const float fov = 90; 
    const float cameraSpeed = 0.05f;
};


#endif