#include "logger.hpp"
#include <iostream>
#include <windows.h>

namespace Logger {

    void logWithColor(const std::string& message, print_color color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
        std::cout << "PluginLoader: " << message << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(print_color::white));
    }

    void log(const std::string& message, print_color color) {
        logWithColor(message, color);
    }

    void warning(const std::string& message) {
        logWithColor(message, print_color::yellow);
    }

    void error(const std::string& message) {
        logWithColor(message, print_color::red);
    }

} // namespace Logger
