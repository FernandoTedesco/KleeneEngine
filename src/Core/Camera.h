#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <SDL.h>

class Camera{

    public:
    Camera();
    void ProcessInput();

    private:
    glm::mat4 cameraView;
    glm::mat4 cameraModel;
    glm::mat4 cameraProjection;

    glm::vec3 cameraPos;
    glm::vec3 directionVector;
    glm::vec3 upVector;

    //Camera parameters
    float fov; 
    float yaw; //horizontal rotation XZ
    float pitch; //vertical rotation YZ
};


#endif