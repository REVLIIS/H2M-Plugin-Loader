#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <string>

namespace PluginScanner {

    enum class ScanStatus {
        Success,
        NoPluginsFolder,
        NoPluginsFound,
        ScanFailed
    };

    ScanStatus initialize();
    void start();
    void shutdown();
    const std::vector<std::string>& get_plugin_paths();

} // namespace PluginScanner