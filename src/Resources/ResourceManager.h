#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <map>
class Mesh;
class Texture;
class ResourceManager{

    public:
    ResourceManager();
    ~ResourceManager();
    uint32_t CreateMesh(const std::string& name ,std::filesystem::path filePath);
    Mesh* GetMesh(uint32_t id);
    Texture* GetTexture(uint32_t id);
    uint32_t CreateTexture(const std::string& name ,std::filesystem::path filePath);

    
    static std::filesystem::path FolderFinder(const std::string& foldername);
    private:
    std::map<std::string, uint32_t> meshMap;
    std::map<std::string, uint32_t> textureMap;
    std::vector<Mesh*> meshVector;
    std::vector<Texture*> textureVector;

    
};
