#pragma once

#include <windows.h>
#include <vector>
#include <string>

namespace PluginInjector {
    void initialize(const std::vector<std::string>& pluginPaths);
    void inject_plugins();

    void inject_dll(std::string dllPath);
    void detach_dll(HMODULE hModule);
}
