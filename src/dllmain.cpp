#include <filesystem>

#include "Logger.hpp"
#include "d3d11proxy.hpp"
#include "PluginScanner.hpp"
#include "PluginInjector.hpp"
#include "HotReloader.hpp"
#include "Config.hpp"
#include "Version.hpp"


DWORD WINAPI Start(LPVOID /*lpParam*/)
{
    Logger::log("H2M-Mod Plugin Loader (v"+ VERSION_STRING +") By Silverr - https://github.com/revliis/H2M-Plugin-Loader/", print_color::light_cyan);

    if (!Config::load()){
        Logger::error("failed to load config, using defaults");
    }

    // Not really important, but just to let users know
    char processPath[MAX_PATH];
    if (GetModuleFileNameA(NULL, processPath, MAX_PATH)) {
        std::string processName = std::filesystem::path(processPath).filename().string();

        if (processName != "h2m-mod.exe") {
            Logger::warning("DLL injected into an unexpected process: " + processName);
        }
    }
    else {
        Logger::error("Failed to retrieve a process name.");
    }

    if (!d3d11proxy::initialize()) {
        Logger::error("Failed to create d3d11 proxy.");
        return 0;
    }

    if (PluginScanner::initialize() != PluginScanner::ScanStatus::Success) {
        Logger::error("No plugins to load, aborting.");
        return 0;
    }

    PluginInjector::initialize(PluginScanner::get_plugin_paths());

    Logger::log("Done loading all plugins!", print_color::light_green);

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, Start, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        PluginScanner::shutdown();
        HotReloader::shutdown();
        d3d11proxy::shutdown();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
