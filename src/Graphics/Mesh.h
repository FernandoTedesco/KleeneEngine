#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <string>

class Mesh{

    public:
    Mesh();
    struct Vertex{
        glm::vec3 pos;
      
    };
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indexes;
    void Draw();
    void SetupMesh();
    bool LoadOBJ(const std::string& filepath);
    

    private:
    unsigned int VAO, VBO, EBO;
    
};

#endif