#include "Paths.h"
#include "Resources/ResourceManager.h"

std::filesystem::path Paths::Root;
std::filesystem::path Paths::Assets;
std::filesystem::path Paths::Models;
std::filesystem::path Paths::Textures;
std::filesystem::path Paths::Scenes;
std::filesystem::path Paths::Tilemaps;

void Paths::Init()
{
    Root = ResourceManager::FolderFinder("assets");
    Assets = Root / "assets";
    Models = Assets / "models";
    Textures = Assets / "textures";
    Scenes = Assets / "scenes";
    Tilemaps = Assets / "tilemaps";
}