#include <vector>
#include <glad/glad.h>
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

Mesh::Mesh(){
    VAO = 0;
    VBO = 0;
    EBO = 0;
}


void Mesh::SetupMesh(){
    
 //Configuration for data delivery
 glGenVertexArrays(1, &this->VAO);
 glGenBuffers(1, &this->VBO);
 glGenBuffers(1, &this->EBO);
 glBindVertexArray(this->VAO);
 glBindBuffer(GL_ARRAY_BUFFER, VBO);

 
 //Data delivery RAM->VRAM
 glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)0);
 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)offsetof(Mesh::Vertex, textureCoordinates));
 glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)offsetof(Mesh::Vertex, normal));

 //EBO Initialization
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indexes[0])*this->indexes.size(), this->indexes.data(), GL_STATIC_DRAW);

 //VAO detachment
 glEnableVertexAttribArray(0);
 glEnableVertexAttribArray(1);
 glEnableVertexAttribArray(2);
 glBindBuffer(GL_ARRAY_BUFFER, 0);
 glBindVertexArray(0);


}

void Mesh::Draw()
{

    //Activate VAO stage
    glBindVertexArray(this->VAO); 
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indexes.size()),GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}

bool Mesh::LoadOBJ(const std::string& filepath)
{
    

    std::ifstream objFile;
    
    unsigned int index1,index2,index3;
    std::string prefix;

    
    std::vector<unsigned int> indexContainer;

    std::vector<glm::vec3> positionContainer;
    std::vector<glm::vec2> textureCoordinateContainer;
    std::vector<glm::vec3> normalContainer;
    std::string line;
    
    objFile.open(filepath);
    if(objFile.is_open())
    {
     

     while(std::getline(objFile, line))
     {
        
        std::stringstream objStream(line);
        objStream >> prefix;

        if(prefix == "v")
        {
         
         //Vertex Reader
         glm::vec3 pos;
         objStream >> pos.x;
         objStream >> pos.y;
         objStream >> pos.z;
         
         positionContainer.push_back(pos);

        
        }
        else if(prefix == "vt")
        {   
            glm::vec2 uv;
            objStream >> uv.x;
            objStream >> uv.y;
        
            textureCoordinateContainer.push_back(uv);
            
            

        }
        else if(prefix == "vn")
        {
            glm::vec3 normal;
            objStream >> normal.x;
            objStream >> normal.y;
            objStream >> normal.z;
            normalContainer.push_back(normal);
        }
        else if(prefix == "f")
        {
          //Main unifier of mesh data
          unsigned int posIndex, uvIndex, normalIndex;
          char slash;
          for(int i=0;i<3;i++)
          {
            objStream >> posIndex >> slash >> uvIndex >> slash >> normalIndex;
            Vertex vertex;
            vertex.position = positionContainer[posIndex - 1];
            vertex.textureCoordinates =  textureCoordinateContainer[uvIndex - 1];
            vertex.normal =  normalContainer[normalIndex - 1];

            this->vertices.push_back(vertex);
            this->indexes.push_back(this->vertices.size()-1);


          }
        }
     }
     
     
     objFile.close();
     return(true);
    }
    else
    {

     return false;
    }
    

}