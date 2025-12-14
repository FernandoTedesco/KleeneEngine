#include <vector>
#include <glad/glad.h>
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>


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

 //EBO Initialization
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indexes[0])*this->indexes.size(), this->indexes.data(), GL_STATIC_DRAW);

 //VAO detachment
 glEnableVertexAttribArray(0);
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

    std::vector<Vertex> vertexContainer;
    std::vector<unsigned int> indexContainer;
    
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
         Vertex v;
         objStream >> v.pos.x;
         objStream >> v.pos.y;
         objStream >> v.pos.z;
         
         vertexContainer.push_back(v);

        
        }
        else if(prefix == "f")
        {
         //Index Reader
         objStream >> index1;
         objStream >> index2;
         objStream >> index3;
         indexContainer.push_back(index1 - 1);
         indexContainer.push_back(index2 - 1);
         indexContainer.push_back(index3 - 1);
        }
     }
     this->vertices = vertexContainer;
     this->indexes = indexContainer;
     objFile.close();
     return(true);
    }
    else
    {

     return false;
    }
    

}