#pragma once
#include <filesystem>
#include <string>

struct Paths {
    static std::filesystem::path Root;
    static std::filesystem::path Assets;
    static std::filesystem::path Models;
    static std::filesystem::path Textures;
    static std::filesystem::path Scenes;
    static std::filesystem::path Tilemaps;

    static void Init();
};