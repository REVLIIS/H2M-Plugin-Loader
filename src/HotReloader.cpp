#include <thread>
#include <chrono>

#include <atomic>
#include "HotReloader.hpp"
#include "Logger.hpp"
#include "PluginInjector.hpp"
#include "Config.hpp"

namespace HotReloader {

    bool initialized = false;
    std::unordered_map<std::string, HotreloadFile> loadedPlugins;
    const std::filesystem::path basePath = std::filesystem::current_path();
    const std::filesystem::path pluginsPath = basePath / Config::PluginScanner.pluginFolder;
    const std::filesystem::path tempDir = pluginsPath / Config::PluginScanner.tempFolder;

    std::atomic<bool> keepRunning{ true };
    std::unique_ptr<std::thread> watcherThread;


    std::string to_relative_path(const std::filesystem::path& fullPath) {
        return std::filesystem::relative(fullPath, basePath).string();
    }

    void store_hmodule(std::string relativePath, HMODULE hModule) {
        if (loadedPlugins.count(relativePath) > 0) {
            loadedPlugins[relativePath].hModule = hModule;
        }
        else {
            Logger::error("HotReloader failed to update HMODULE for: " + relativePath);
        }
    }

    void unload_plugin(std::string relativePath) {
        Logger::log("HotReloader is attempting to unload the plugin " + relativePath, print_color::gray);

        if (loadedPlugins.count(relativePath) > 0) {
            PluginInjector::detach_dll(loadedPlugins[relativePath].hModule);
            loadedPlugins.erase(relativePath);
        }
        else {
            Logger::error("HotReloader: No loaded plugin with that path " + relativePath);
        }
    }

    void load_plugin(std::string relativePath)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::HotReload.hotReloadDelayInMSEC));

        PluginInjector::inject_dll(relativePath);
    }

    void watch_plugin_folder() {
        std::wstring directory = pluginsPath.wstring();

        HANDLE hDir = CreateFileW(
            directory.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr
        );

        if (hDir == INVALID_HANDLE_VALUE) {
            Logger::error("HotReloader failed to open directory handle.");
            return;
        }

        char buffer[1024];
        DWORD bytesReturned;
        FILE_NOTIFY_INFORMATION* pNotify;
        std::wstring fileName;
        std::unordered_map<std::wstring, std::chrono::steady_clock::time_point> suppressedFiles;
        const std::chrono::milliseconds suppressionWindow(Config::HotReload.eventSuppressionWindowInMSEC + Config::HotReload.hotReloadDelayInMSEC); // Suppression window to prevent multiple detections e.g. when adding a file getting 2 modified calls after

        while (keepRunning) {
            if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned, nullptr, nullptr)) {

                pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
                do {
                    fileName.assign(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));

                    bool shouldIgnore = fileName[0] == L'.' || std::filesystem::path(fileName).extension() != L".dll";
                    if (!shouldIgnore) {
                        auto now = std::chrono::steady_clock::now();
                        if (suppressedFiles.count(fileName) == 0 || now - suppressedFiles[fileName] > suppressionWindow) {
                            std::filesystem::path fullPath = pluginsPath / fileName;
                            std::string relativePath = to_relative_path(fullPath);

                            switch (pNotify->Action) {
                            case FILE_ACTION_ADDED:
                                Logger::log("HotReloader detected a new plugin " + relativePath, print_color::magenta);
                                load_plugin(relativePath);
                                break;
                            case FILE_ACTION_REMOVED:
                                Logger::log("HotReloader detected plugin deletion " + relativePath, print_color::magenta);
                                unload_plugin(relativePath);
                                break;
                            case FILE_ACTION_MODIFIED:
                                Logger::log("HotReloader detected a plugin modification " + relativePath, print_color::magenta);
                                unload_plugin(relativePath);
                                load_plugin(relativePath);
                                break;
                            }

                            suppressedFiles[fileName] = now;
                        }
                    }

                    pNotify = pNotify->NextEntryOffset
                        ? reinterpret_cast<FILE_NOTIFY_INFORMATION*>((char*)pNotify + pNotify->NextEntryOffset)
                        : nullptr;

                } while (pNotify);
            }
            else {
                Logger::error("HotReloader failed to read directory changes.");
                break;
            }
        }

        CloseHandle(hDir);
    }

    void initialize() {
        if (initialized) 
            return;

        Logger::log("HotReloader initialized.", print_color::light_magenta);

        // Clean up the temp directory
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }

        std::filesystem::create_directories(tempDir);

        watcherThread = std::make_unique<std::thread>(watch_plugin_folder);

        initialized = true;
    }

    std::string initialize_for_plugin(std::string pluginPath) {
        if (!initialized)
            initialize();

        // Generate name for temp dll
        std::string tempFileName = std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".dll";
        std::filesystem::path tempFilePath = tempDir / tempFileName;

        // Copy the DLL to the temp dir
        std::filesystem::copy_file(pluginPath, tempFilePath, std::filesystem::copy_options::overwrite_existing);

        // Convert paths to relative paths
        std::string relativeTempPath = to_relative_path(tempFilePath);

        // Track the plugin using relative paths
        loadedPlugins[pluginPath] = { pluginPath, relativeTempPath, nullptr };

        return relativeTempPath;
    }

    void shutdown() {
        if (!initialized) {
            return;
        }

        Logger::log("Shutting down HotReloader...", print_color::light_magenta);
        keepRunning = false;  // Signal the watcher thread to stop

        if (watcherThread && watcherThread->joinable()) {
            watcherThread->join();  // Wait for the thread to finish
        }

        watcherThread.reset();  // Clean up the thread object
        initialized = false;

        Logger::log("HotReloader shutdown complete.", print_color::green);
    }
} // namespace HotReloader
