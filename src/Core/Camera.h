#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <SDL.h>
#include "Input.h"

class Camera{

    public:
    Camera();
    void ProcessInput();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    private:
    glm::mat4 cameraViewMatrix;
    glm::mat4 cameraProjectionMatrix;

    glm::vec3 cameraPos;
    glm::vec3 directionVector;
    glm::vec3 upVector;

    //Camera parameters
    float fov; 
    float yaw; //horizontal rotation XZ
    float pitch; //vertical rotation YZ
    float cameraSpeed;
};


#endif