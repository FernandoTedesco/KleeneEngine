
#include "StbImage/stb_image.h"

#include <glad/glad.h>
#include <iostream>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Shader.h"
#include <filesystem>
#include "Core/Window.h"
#include "Graphics/Mesh.h"
#include "Resources/ResourceManager.h"
#include "Core/Camera.h"
#include "Core/Input.h"
#include <vector>
#include "Resources/Texture.h"
int main(int argc, char* argv[]){

   //Assets Folder Finder
   std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
   //Window, Shader & Test mesh Initialization
   Window window;
   Shader shader((currentPath/"assets/shaders/core.vert").string(), (currentPath/"assets/shaders/core.frag").string());
   Camera camera;
   Mesh mesh;
   mesh.LoadOBJ((currentPath/"assets/models/plane.obj").string());
   mesh.SetupMesh();
   Texture texture1;
   texture1.LoadTexture(currentPath/"assets/textures/testgrass.jpg");
   shader.Use();
   shader.SetInt("texture1", 0);

   std::vector<glm::vec3> positions;
   positions.push_back(glm::vec3(0.0f,0.0f,0.0f));
   
   std::vector<glm::vec3> colors;
   colors.push_back(glm::vec3(0.4,0.4,1));
   

   glEnable(GL_DEPTH_TEST);
   //Main Loop
   bool isrunning = true;
   
     while(isrunning)
     {
        
        isrunning=window.ProcessEvents();
        camera.ProcessInput();
        window.GetHeight();
        float screenWidth = (float)window.GetWidth();
        float screenHeight = (float)window.GetHeight();
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        glm::mat4 projectionMatrix = camera.GetProjectionMatrix(screenWidth, screenHeight);
        
         
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

        shader.Use();
        
        
        
        
        
        
        
        shader.SetMat4("view",viewMatrix);
        shader.SetMat4("projection",projectionMatrix);
         
        //Draw mesh
        for(unsigned int i=0; i<positions.size(); i++)
        {
         
         glm::mat4 model = glm::mat4(1.0f); 
         
         shader.SetMat4("model",model);
         texture1.Use(0);
         mesh.Draw();
        }
        
        Input::ResetMouseDelta();
        window.SwapBuffers();

     }

  return 0;
  
}