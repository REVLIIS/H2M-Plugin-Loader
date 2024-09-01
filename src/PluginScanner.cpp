#include "PluginScanner.hpp"

#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <stack>

#include "Logger.hpp"
#include "config.hpp"

namespace fs = std::filesystem;

namespace {

    std::string plugins_dir;
    std::vector<std::string> plugin_paths;
    std::unique_ptr<std::thread> scan_thread;

    bool DirectoryExists(const std::string& dirPath) {
        return fs::exists(dirPath) && fs::is_directory(dirPath);
    }

    void ScanDirectory(const std::string& directory, std::vector<std::string>& files) {
        std::stack<fs::path> directories;
        directories.push(directory);

        while (!directories.empty()) {
            fs::path currentDir = directories.top();
            directories.pop();

            for (const auto& entry : fs::directory_iterator(currentDir)) {
                if (entry.is_directory()) {
                    std::string folderName = entry.path().filename().string();
                    if (folderName[0] == '.') {
                        Logger::log("Ignoring directory: " + entry.path().string(), print_color::yellow);
                        continue;
                    }
                    directories.push(entry.path());
                }
                else if (entry.path().extension() == ".dll") {
                    files.push_back(entry.path().string());
                }
            }
        }
    }

    PluginScanner::ScanStatus scan_for_plugins() {
        plugin_paths.clear();

        Logger::log("Scanning for plugins...", print_color::bright_white);

        if (!DirectoryExists(plugins_dir)) {
            std::string fullPath = fs::absolute(plugins_dir).string();
            Logger::error("Plugins directory does not exist: '" + fullPath + "'");
            return PluginScanner::ScanStatus::NoPluginsFolder;
        }

        ScanDirectory(plugins_dir, plugin_paths);

        if (plugin_paths.empty()) {
            Logger::log("No plugins found in '" + plugins_dir + "' Injection process aborted.", print_color::light_red);
            return PluginScanner::ScanStatus::NoPluginsFound;
        }

        Logger::log("Total plugins found: " + std::to_string(plugin_paths.size()), print_color::green);

        // Just checking if the binary is loaded by checking if the address has a value, don't even know which one anymore, think it was fs_basepath
        uint64_t addressToCheck = Config::PluginScanner.pointerScanAddress;
        void** ptr = reinterpret_cast<void**>(addressToCheck);
        Logger::warning("Waiting for the game to be fully loaded...");

        int maxRetries = Config::PluginScanner.maxScanRetries;
        int maxRetriesWaitInMSEC = Config::PluginScanner.scanRetryWaitInMSEC;
        int retryCount = 0;

        // Keep checking the pointer
        while (*ptr == nullptr && retryCount < maxRetries) {
            std::this_thread::sleep_for(std::chrono::milliseconds(maxRetriesWaitInMSEC));
            retryCount++;
        }

        if (retryCount == maxRetries) {
            Logger::error("Failed to detect a game instance after "+ std::to_string(maxRetries) + " attempts. Aborting plugin load.");
            return PluginScanner::ScanStatus::ScanFailed;
        }

        Logger::log("Game is ready. Proceeding with loading plugins.", print_color::green);

        return PluginScanner::ScanStatus::Success;
    }

} // unnamed namespace

namespace PluginScanner {

    ScanStatus initialize() {
        plugins_dir = Config::PluginScanner.pluginFolder;
        return scan_for_plugins();
    }

    const std::vector<std::string>& get_plugin_paths() {
        return plugin_paths;
    }

    void shutdown() {
        if (scan_thread && scan_thread->joinable()) {
            scan_thread->join();
        }
        scan_thread.reset();
    }

} // namespace PluginScanner
