
#include <glm/glm.hpp>
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>



Camera::Camera(){

    //Camera Initialization 
    
    

    cameraPos = glm::vec3(0,0,3.0f);
    directionVector = glm::vec3(0,0,-1.0f);
    upVector = glm::vec3(0,1.0f,0);
    yaw = -90;
    pitch = 0.0f;

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

    //Mouse update
    float sensitivity = 0.2f;
    float xOffset = Input::GetMouseDeltaX() * sensitivity;
    float yOffset = Input::GetMouseDeltaY() * sensitivity;
    yaw+=xOffset;
    pitch-=yOffset;

    if(pitch>89.0f) pitch =89.f;
    if(pitch<-89.0f)pitch = -89.f;
    
    //New Direction
    glm::vec3 newDirection;
    newDirection.x = cos(glm::radians(pitch))*cos(glm::radians(yaw));
    newDirection.y = sin(glm::radians(pitch));
    newDirection.z = cos(glm::radians(pitch))*sin(glm::radians(yaw));
    this->directionVector = glm::normalize(newDirection);
    
    //Keyboard movement
    if(Input::IsKeyDown(Input::W_KEY))
    {
        cameraPos += (directionVector * cameraSpeed);
    }
    if(Input::IsKeyDown(Input::S_KEY))
    {
        cameraPos -= (directionVector * cameraSpeed);
    }
    if(Input::IsKeyDown(Input::A_KEY))
    {
        rightVector = glm::normalize(glm::cross(upVector, directionVector));
        cameraPos += (rightVector * cameraSpeed);
    }
    if(Input::IsKeyDown(Input::D_KEY))
    {
        rightVector = glm::normalize(glm::cross(upVector, directionVector));
        cameraPos -= (rightVector * cameraSpeed);
    }
}
