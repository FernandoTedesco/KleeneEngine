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

int main(int argc, char* argv[]){

   //Assets Folder Finder
   std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
   //Window, Shader & Test mesh Initialization
   Window window;
   Shader shader((currentPath/"assets/shaders/core.vert").string(), (currentPath/"assets/shaders/core.frag").string());
   Camera camera;
   Mesh mesh;
   mesh.LoadOBJ((currentPath/"assets/models/pyramid.obj").string());
   mesh.SetupMesh();

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
        
        
        //Test transformations
        glm::mat4 model = glm::mat4(1.0f); 
        model = glm::rotate(model,(float)SDL_GetTicks()/1000.0f,glm::vec3(0.5f, 1.0f, 0.0f)); 

        shader.SetMat4("model",model);
        shader.SetMat4("view",viewMatrix);
        shader.SetMat4("projection",projectionMatrix);
         
        //Draw mesh
        mesh.Draw();
        window.SwapBuffers();

     }

  return 0;
  
}