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

int main(int argc, char* argv[]){

   //Assets Folder Finder
   std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
   //Window, Shader & Test mesh Initialization
   Window window;
   Shader shader((currentPath/"assets/shaders/core.vert").string(), (currentPath/"assets/shaders/core.frag").string());
   Mesh mesh;
   mesh.LoadOBJ((currentPath/"assets/models/pyramid.obj").string());
   mesh.SetupMesh();

   glEnable(GL_DEPTH_TEST);
   //Main Loop
   bool isrunning = true;
     while(isrunning)
     {
        
        isrunning=window.ProcessEvents();

        
        {

        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

        shader.Use();
        
        
        //Test transformations
        glm::mat4 model = glm::mat4(1.0f); 
        model = glm::rotate(model,(float)SDL_GetTicks()/1000.0f,glm::vec3(0.5f, 1.0f, 0.0f)); 
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        
        shader.SetMat4("model",model);
        shader.SetMat4("view",view);
        shader.SetMat4("projection",projection);
         
        //Draw mesh
        mesh.Draw();
        window.SwapBuffers();

     }

  return 0;
  
}