#include "Paths.h"
#include "Resources/ResourceManager.h"

std::filesystem::path Paths::Root;
std::filesystem::path Paths::Assets;
std::filesystem::path Paths::Models;
std::filesystem::path Paths::Textures;

void Paths::Init()
{
    Root = ResourceManager::FolderFinder("assets");
    Assets = Root / "assets";
    Models = Assets / "models";
    Textures = Assets / "textures";
}