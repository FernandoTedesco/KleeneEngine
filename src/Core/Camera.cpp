#include <SDL.h>
#include <glm/glm.hpp>
#include "Window.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>



Camera::Camera(){

    //Camera Initialization 
    
    

    cameraPos = glm::vec3(0,0,3.0f);
    directionVector = glm::vec3(0,0,-1.0f);
    upVector = glm::vec3(0,1.0f,0);
    yaw = -90;
    pitch = 0.0f;
    fov = 90;
    cameraSpeed = 0.05f;
    


}

glm::mat4 Camera::GetViewMatrix(){

    cameraViewMatrix = glm::lookAt(cameraPos,cameraPos + directionVector, upVector);
    return this->cameraViewMatrix;
}
glm::mat4 Camera::GetProjectionMatrix(float screenWidth, float screenHeight){

    cameraProjectionMatrix = glm::perspective(glm::radians(this->fov), screenWidth/screenHeight,0.1f, 100.0f);
    return this->cameraProjectionMatrix;
}

void Camera::ProcessInput(){

    if(Input::IsKeyDown(SDL_SCANCODE_W))
    {
        cameraPos += (directionVector * cameraSpeed);
    }
    if(Input::IsKeyDown(SDL_SCANCODE_S))
    {
        cameraPos -= (directionVector * cameraSpeed);
    }
    if(Input::IsKeyDown(SDL_SCANCODE_A))
    {
        rightVector = glm::cross(upVector, directionVector);
        cameraPos += (rightVector * cameraSpeed);
    }
    if(Input::IsKeyDown(SDL_SCANCODE_D))
    {
        rightVector = glm::cross(upVector, directionVector);
        cameraPos -= (rightVector * cameraSpeed);
    }
}
