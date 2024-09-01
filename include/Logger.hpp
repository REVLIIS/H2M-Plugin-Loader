#pragma once

#include <string>
#include <windows.h>

enum class print_color {
    black = 0,
    blue = 1,
    green = 2,
    cyan = 3,
    red = 4,
    magenta = 5,
    yellow = 6,
    white = 7,
    gray = 8,
    light_blue = 9,
    light_green = 10,
    light_cyan = 11,
    light_red = 12,
    light_magenta = 13,
    light_yellow = 14,
    bright_white = 15
};

namespace Logger {
    void log(const std::string& message, print_color color = print_color::white);
    void warning(const std::string& message);
    void error(const std::string& message);
}
