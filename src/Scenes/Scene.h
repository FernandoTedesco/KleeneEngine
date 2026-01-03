#pragma once
#include <vector>
#include "glm/glm.hpp"
#include <string>
#include "GameObject.h"
#include "Graphics/Light.h"
#include "Graphics/Skybox.h"
class Scene
{
    friend class SceneLoader;

public:
    Scene() : skybox(nullptr) {};
    std::vector<glm::vec3> scenePositions;
    std::vector<glm::vec3> sceneScales;
    std::vector<glm::vec4> sceneRotations;
    std::vector<uint32_t> sceneMeshes;
    std::vector<uint32_t> sceneTextures;
    std::vector<GameObject> gameObjects;
    std::vector<Light> lights;
    Skybox* skybox;
    std::vector<std::string> skyboxPaths;

    ~Scene()
    {
	if (skybox)
	{
	    delete skybox;
	    skybox = nullptr;
	}
    };
};