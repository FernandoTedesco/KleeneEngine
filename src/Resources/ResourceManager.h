#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <map>

class Mesh;
class Texture;
class Material;

class ResourceManager
{

public:
    ResourceManager();
    ~ResourceManager();

    // Create new pointers for each major resource type
    uint32_t CreateMesh(const std::string& name, std::filesystem::path filePath);
    uint32_t CreateMesh(const std::string& name);
    uint32_t CreateTexture(const std::string& name, std::filesystem::path filePath);
    uint32_t CreateMaterial(const std::string& name, uint32_t textureID);

    // Get the resource pointers from their ID;
    Mesh* GetMesh(uint32_t id);
    Texture* GetTexture(uint32_t id);
    Material* GetMaterial(uint32_t id);

    // Auxiliary function for finding the path to any folder in the project
    static std::filesystem::path FolderFinder(const std::string& foldername);

    // Vector for the names of each resource (position of name = ID)
    std::vector<std::string> meshNames;
    std::vector<std::string> textureNames;
    std::vector<std::string> materialNames;

private:
    // Get a resource ID through a name
    std::map<std::string, uint32_t> meshMap;
    std::map<std::string, uint32_t> textureMap;
    std::map<std::string, uint32_t> materialMap;

    // Real resource Allocation
    std::vector<Mesh*> meshVector;
    std::vector<Texture*> textureVector;
    std::vector<Material*> materialVector;
};
