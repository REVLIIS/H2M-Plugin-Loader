#include "d3d11proxy.hpp"
#include <iostream>
#include <array>
#include <Logger.hpp>

HMODULE realD3D11Module = nullptr;
std::array<UINT64, 51> d3d11FuncPtrs = { 0 };

#define X(index, name) {index, #name},
const FunctionEntry functionEntries[] = {
    FUNCTION_LIST
};
#undef X

namespace d3d11proxy {

    bool initialize()
    {
        char dllpath[MAX_PATH];
        if (GetSystemDirectory(dllpath, MAX_PATH) == 0) {
            Logger::error("Failed to get system directory.");
            return false;
        }

        strcat_s(dllpath, "\\d3d11.dll");
        realD3D11Module = LoadLibrary(dllpath);

        if (!realD3D11Module) {
            Logger::error("Failed to load original d3d11.dll.");
            return false;
        }

        for (const auto& entry : functionEntries) {
            d3d11FuncPtrs[entry.index] = reinterpret_cast<UINT64>(GetProcAddress(realD3D11Module, entry.name));
            if (!d3d11FuncPtrs[entry.index]) {
                Logger::error("Failed to get the address for " + std::string(entry.name) + ".");
                return false;
            }
        }

        return true;
    }

    void shutdown()
    {
        if (realD3D11Module) {
            FreeLibrary(realD3D11Module);
            realD3D11Module = nullptr;
        }
    }

#ifdef __GNUC__
#define GENERATE_FUNCTION(index, name) \
    extern "C" __declspec(dllexport) void name() \
    { \
        asm volatile ( \
            "movq %0, %%rax\n\t" \
            "jmp *%%rax\n\t" \
            : \
            : "m"(d3d11FuncPtrs[index]) \
            : "rax" \
        ); \
    }

#define X(index, name) GENERATE_FUNCTION(index, name)
    FUNCTION_LIST
#undef X

#endif // __GNUC__

} // namespace d3d11proxy
