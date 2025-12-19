#pragma once
#include <string>
#include <filesystem>
#include <map>
class Mesh;
class Texture;
class ResourceManager{

    public:
    ResourceManager();
    ~ResourceManager();
    Mesh* CreateMesh(std::string& name ,std::filesystem::path filePath);
    Mesh* GetMesh(std::string& name);
    Texture* CreateTexture(std::string& name ,std::filesystem::path filePath);

    
    static std::string path;
    static std::filesystem::path FolderFinder(const std::string& foldername);
    private:
    std::map<std::string, Mesh*> meshMap;
    std::map<std::string, Texture*> textureMap;

    
};
