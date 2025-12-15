#include <SDL.h>
#include <glm/glm.hpp>
#include "Window.h"



Camera::Camera(){

    //Camera Initialization 
    
    cameraView = glm::mat4(1.0f);
    cameraModel = glm::mat4(1.0f);
    cameraProjection = glm::mat4(1.0f);
    cameraPos = glm::vec3(0f,0f,3.0f);

    


    




}


void Camera::ProcessInput(){









}
