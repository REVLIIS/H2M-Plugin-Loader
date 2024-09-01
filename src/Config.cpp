#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <functional>

#include "Logger.hpp"

namespace Config {

    HotReloadConfig HotReload;
    PluginScannerConfig PluginScanner;
    PluginInjectorConfig PluginInjector;

    std::string configFileName = "pluginloader.ini";

    std::string trim(const std::string& str) {
        const std::string whitespace = " \t\n\r";
        const size_t start = str.find_first_not_of(whitespace);
        const size_t end = str.find_last_not_of(whitespace);
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    bool load() {

        std::ifstream file(configFileName);

        if (!file.is_open()) {
            Logger::error("Failed to open config file: " + configFileName);
            return false;
        }

        // Define mapping of sections and keys to corresponding actions
        std::unordered_map<std::string, std::function<void(const std::string&)>> hotReloadMap = {
            {"enabled", [](const std::string& value) { HotReload.enabled = (value == "true" || value == "1"); }},
            {"eventSuppressionWindowInMSEC", [](const std::string& value) { HotReload.eventSuppressionWindowInMSEC = std::stoi(value); }},
        };

        std::unordered_map<std::string, std::function<void(const std::string&)>> pluginScannerMap = {
            {"pluginFolder", [](const std::string& value) { PluginScanner.pluginFolder = value; }},
            {"tempFolder", [](const std::string& value) { PluginScanner.tempFolder = value; }},
            {"maxScanRetries", [](const std::string& value) { PluginScanner.maxScanRetries = std::stoi(value); }},
            {"scanRetryWaitInMSEC", [](const std::string& value) { PluginScanner.scanRetryWaitInMSEC = std::stoi(value); }},
            {"pointerScanAddress", [](const std::string& value) { PluginScanner.pointerScanAddress = std::stoull(value, nullptr, 16); }},
        };

        std::unordered_map<std::string, std::function<void(const std::string&)>> pluginInjectorMap = {
            {"maxDetachRetries", [](const std::string& value) { PluginInjector.maxDetachRetries = std::stoi(value); }},
            {"detachRetryWaitInMSEC", [](const std::string& value) { PluginInjector.detachRetryWaitInMSEC = std::stoi(value); }},
        };

        std::string line, section;
        while (std::getline(file, line)) {

            // Remove lines starting with these symbols
            line = line.substr(0, line.find(';'));
            line = line.substr(0, line.find('#'));
            line = trim(line);

            if (line.empty()) continue;

            if (line.front() == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
            }
            else {
                auto delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = trim(line.substr(0, delimiterPos));
                    std::string value = trim(line.substr(delimiterPos + 1));

                    // Dispatch based on section/header
                    if (section == "HotReloader" && hotReloadMap.count(key)) {
                        hotReloadMap[key](value);
                    }
                    else if (section == "PluginScanner" && pluginScannerMap.count(key)) {
                        pluginScannerMap[key](value);
                    }
                    else if (section == "PluginInjector" && pluginInjectorMap.count(key)) {
                        pluginInjectorMap[key](value);
                    }
                }
            }
        }
        return true;
    }
}
