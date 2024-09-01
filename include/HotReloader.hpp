#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <unordered_map>

namespace HotReloader {

    struct HotreloadFile {
        std::string originalPath; // Path to the original plugin
        std::string tempPath;     // Path to the temporary copy in temp plugin folder
        HMODULE hModule;          // Handle of the loaded DLL
    };

    extern std::unordered_map<std::string, HotreloadFile> loadedPlugins;

    std::string initialize_for_plugin(std::string);
    void initialize();
    void shutdown();
    void store_hmodule(const std::string, HMODULE);
}