#pragma once
#include "TestScene.h"
#include "Resources/ResourceManager.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"

TestScene::TestScene(ResourceManager* resourceManager)
{
    
    sceneMeshes.push_back(this->resourceManager->CreateMesh());
}


    void TestScene::Init()
    {
        std::filesystem::path currentPath = ResourceManager::FolderFinder("assets");
        Mesh* mesh = CreateMesh("plane",currentPath/"assets/models/plane.obj");
        Texture* texture = CreateTexture("grass", currentPath/"assets/textures/testgrass.jpg")
    }

    void TestScene::Update(float deltaTime){
        
    }
    
    
    void TestScene::Render()
    {
        for
    }