cmake_minimum_required(VERSION 3.8)

project("H2M-Plugin-Loader")

# Ensure C++17 or later is used for std::filesystem support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Set the compiler to GCC and G++
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# Include the directory where the headers are located
include_directories(include)

# Add source files to this project's shared library (DLL)
add_library(H2M-Plugin-Loader SHARED 
    "src/dllmain.cpp" 
    "src/d3d11proxy.cpp" 
    "src/Logger.cpp" 
    "src/PluginScanner.cpp" 
    "src/PluginInjector.cpp" 
    "src/HotReloader.cpp"
 "src/Config.cpp")

# Apply optimizations for Release mode and include runtime libraries
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -flto -static-libgcc -static-libstdc++")
endif()

# Link against stdc++fs if needed (primarily for older GCC versions)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(H2M-Plugin-Loader stdc++fs)
endif()

# Set the output name of the DLL to "d3d11" without a "lib" prefix
set_target_properties(H2M-Plugin-Loader PROPERTIES 
    OUTPUT_NAME "d3d11"
    PREFIX ""
)

# Set output directories for DLL and associated files
set_target_properties(H2M-Plugin-Loader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
