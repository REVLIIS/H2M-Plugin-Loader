#pragma once
#include <string>
#include <unordered_map>
#include <functional>

namespace Config {

    struct HotReloadConfig {
        bool enabled = false;
        int eventSuppressionWindowInMSEC = 500;
        int hotReloadDelayInMSEC = 0;
    };

    struct PluginScannerConfig {
        std::string pluginFolder = "plugins";
        std::string tempFolder = ".temp";
        int maxScanRetries = 30;
        int scanRetryWaitInMSEC = 1000;
        uint64_t pointerScanAddress = 0x142EC86E0;
    };

    struct PluginInjectorConfig {
        int maxDetachRetries = 10;
        int detachRetryWaitInMSEC = 250;
    };

    extern HotReloadConfig HotReload;
    extern PluginScannerConfig PluginScanner;
    extern PluginInjectorConfig PluginInjector;

    bool load();
}
