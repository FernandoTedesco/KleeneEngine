#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "GameObject.h"
class Scene{
    friend class SceneLoader;
    public:
    Scene(){};
    std::vector<glm::vec3> scenePositions;
    std::vector<glm::vec3> sceneScales;
    std::vector<glm::vec4> sceneRotations;
    std::vector<uint32_t> sceneMeshes;
    std::vector<uint32_t> sceneTextures;
    std::vector<GameObject> gameObjects;
    
    ~Scene(){};



};