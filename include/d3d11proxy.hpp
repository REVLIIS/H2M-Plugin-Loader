#pragma once

#include <basetsd.h>
#include <wtypes.h>

struct FunctionEntry {
    int index;
    const char* name;
};

// List of original d3d11 functions
#define FUNCTION_LIST \
    X(0, CreateDirect3D11DeviceFromDXGIDevice) \
    X(1, CreateDirect3D11SurfaceFromDXGISurface) \
    X(2, D3D11CoreCreateDevice) \
    X(3, D3D11CoreCreateLayeredDevice) \
    X(4, D3D11CoreGetLayeredDeviceSize) \
    X(5, D3D11CoreRegisterLayers) \
    X(6, D3D11CreateDevice) \
    X(7, D3D11CreateDeviceAndSwapChain) \
    X(8, D3D11CreateDeviceForD3D12) \
    X(9, D3D11On12CreateDevice) \
    X(10, D3DKMTCloseAdapter) \
    X(11, D3DKMTCreateAllocation) \
    X(12, D3DKMTCreateContext) \
    X(13, D3DKMTCreateDevice) \
    X(14, D3DKMTCreateSynchronizationObject) \
    X(15, D3DKMTDestroyAllocation) \
    X(16, D3DKMTDestroyContext) \
    X(17, D3DKMTDestroyDevice) \
    X(18, D3DKMTDestroySynchronizationObject) \
    X(19, D3DKMTEscape) \
    X(20, D3DKMTGetContextSchedulingPriority) \
    X(21, D3DKMTGetDeviceState) \
    X(22, D3DKMTGetDisplayModeList) \
    X(23, D3DKMTGetMultisampleMethodList) \
    X(24, D3DKMTGetRuntimeData) \
    X(25, D3DKMTGetSharedPrimaryHandle) \
    X(26, D3DKMTLock) \
    X(27, D3DKMTOpenAdapterFromHdc) \
    X(28, D3DKMTOpenResource) \
    X(29, D3DKMTPresent) \
    X(30, D3DKMTQueryAdapterInfo) \
    X(31, D3DKMTQueryAllocationResidency) \
    X(32, D3DKMTQueryResourceInfo) \
    X(33, D3DKMTRender) \
    X(34, D3DKMTSetAllocationPriority) \
    X(35, D3DKMTSetContextSchedulingPriority) \
    X(36, D3DKMTSetDisplayMode) \
    X(37, D3DKMTSetDisplayPrivateDriverFormat) \
    X(38, D3DKMTSetGammaRamp) \
    X(39, D3DKMTSetVidPnSourceOwner) \
    X(40, D3DKMTSignalSynchronizationObject) \
    X(41, D3DKMTUnlock) \
    X(42, D3DKMTWaitForSynchronizationObject) \
    X(43, D3DKMTWaitForVerticalBlankEvent) \
    X(44, D3DPerformance_BeginEvent) \
    X(45, D3DPerformance_EndEvent) \
    X(46, D3DPerformance_GetStatus) \
    X(47, D3DPerformance_SetMarker) \
    X(48, EnableFeatureLevelUpgrade) \
    X(49, OpenAdapter10) \
    X(50, OpenAdapter10_2)

extern const FunctionEntry functionEntries[];

namespace d3d11proxy {

    bool initialize();
    void shutdown();

#define X(index, name) extern "C" __declspec(dllexport) void name();
    FUNCTION_LIST
#undef X

} // namespace d3d11proxy