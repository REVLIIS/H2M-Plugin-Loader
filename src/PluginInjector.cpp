#include "PluginInjector.hpp"
#include "logger.hpp"
#include "HotReloader.hpp"
#include "Config.hpp"
#include <windows.h>
#include <chrono>
#include <thread>

namespace {
    std::vector<std::string> plugin_paths;
}

namespace PluginInjector {

    void initialize(const std::vector<std::string>& pluginPaths) {
        plugin_paths = pluginPaths;
        inject_plugins();
    }

    void inject_plugins() {
        for (auto dllPath : plugin_paths) {
            inject_dll(dllPath);
        }
    }

    void inject_dll(std::string dllPath) {
        std::string originalPath = dllPath;
        
        //Enable hotreloading by creating a temp copy and invoking a watcher on the original file to check for changes
        if (Config::HotReload.enabled){
            dllPath = HotReloader::initialize_for_plugin(dllPath);
        }

        HMODULE hModule = LoadLibraryA(dllPath.c_str());

        if (!hModule) {
            Logger::error("Failed to load: " + originalPath);
            return;
        }

        //Send created hModule to the HotReloader to keep track of.
        if (Config::HotReload.enabled) {
            HotReloader::store_hmodule(originalPath, hModule);
        }

        Logger::log("Successfully loaded: " + originalPath, print_color::green);
    }

    void detach_dll(HMODULE hModule) {
        if (!hModule) {
            Logger::error("hModule is null. Cannot unload the plugin.");
            return;
        }

        Logger::log("Attempting to unload the plugin...", print_color::yellow);

        FreeLibrary(hModule);

        MEMORY_BASIC_INFORMATION mbi;
        int retryCount = 0;
        const int maxRetries = Config::PluginInjector.maxDetachRetries;
        const int retryWaitTime = Config::PluginInjector.detachRetryWaitInMSEC;

        while (VirtualQuery(hModule, &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT) {
            if (mbi.AllocationBase == hModule) {
                if (retryCount >= maxRetries) {
                    Logger::error("Failed to unload the plugin after 10 attempts.");
                    return;
                }
                // Still in use, wait a bit and retry
                std::this_thread::sleep_for(std::chrono::milliseconds(retryWaitTime));
                retryCount++;
                continue;
            }
            break;
        }

        if (retryCount < maxRetries) {
            Logger::log("Successfully unloaded plugin.", print_color::green);
        }
    }
} // namespace PluginInjector
