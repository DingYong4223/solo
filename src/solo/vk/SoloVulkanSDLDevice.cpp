/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#include "SoloVulkanSDLDevice.h"

#ifdef SL_VULKAN_RENDERER

#include <SDL_syswm.h>
#ifdef SL_WINDOWS
#   include <windows.h>
#endif

using namespace solo;

VulkanSDLDevice::VulkanSDLDevice(const DeviceSetup &setup):
    SDLDevice(setup)
{
    auto flags = static_cast<u32>(SDL_WINDOW_ALLOW_HIGHDPI);
    if (setup.fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window_ = SDL_CreateWindow(setup.windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        setup.canvasWidth, setup.canvasHeight, flags);
    SL_DEBUG_PANIC(!window_, "Unable to create device window");

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "";
    appInfo.pEngineName = "";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    vec<const s8*> enabledExtensions {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef SL_WINDOWS
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#ifdef SL_DEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
    };

    vec<const s8*> enabledLayers {
#ifdef SL_DEBUG
        "VK_LAYER_LUNARG_standard_validation",
#endif
    };

    VkInstanceCreateInfo instanceInfo {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.pApplicationInfo = &appInfo;

    if (!enabledLayers.empty())
    {
        instanceInfo.enabledLayerCount = enabledLayers.size();
        instanceInfo.ppEnabledLayerNames = enabledLayers.data();
    }

    if (!enabledExtensions.empty())
    {
        instanceInfo.enabledExtensionCount = static_cast<u32>(enabledExtensions.size());
        instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();
    }

    instance_ = VulkanResource<VkInstance>{vkDestroyInstance};
    SL_VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, instance_.cleanRef()));

#ifdef SL_WINDOWS
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);

    const auto hwnd = wmInfo.info.win.window;
    const auto hinstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

    VkWin32SurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.flags = 0;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.hinstance = hinstance;
    surfaceInfo.hwnd = hwnd;

    surface_ = VulkanResource<VkSurfaceKHR>{instance_, vkDestroySurfaceKHR};
    SL_VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(instance_, &surfaceInfo, nullptr, surface_.cleanRef()));
#endif
}

VulkanSDLDevice::~VulkanSDLDevice()
{
    cleanupSubsystems();
}

void VulkanSDLDevice::saveScreenshot(const str &path)
{
    // TODO
}

void VulkanSDLDevice::endUpdate()
{
}

#endif
