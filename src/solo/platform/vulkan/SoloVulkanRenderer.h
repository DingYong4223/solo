/*
    Copyright (c) Aleksey Fedotov

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include "SoloRenderer.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloVulkan.h"

namespace solo
{
    class Device;
    class Vector2;

    class VulkanRenderer final: public Renderer
    {
    public:
        explicit VulkanRenderer(Device *device);
        ~VulkanRenderer();

    protected:
        void beginFrame() override final;
        void endFrame() override final;

    private:
        struct SwapchainBuffer
        {
            VkImage image = nullptr;
            VkImageView imageView = nullptr;
        };

        uint32_t canvasWidth = 0;
        uint32_t canvasHeight = 0;
        VkDevice device = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkQueue queue = nullptr;
        VkPhysicalDeviceFeatures features;
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        VkSemaphore presentCompleteSem = nullptr;
        VkSemaphore renderCompleteSem = nullptr;
        VkCommandPool commandPool = nullptr;
        VkRenderPass renderPass = nullptr;
        VkSwapchainKHR swapchain = nullptr;
        std::vector<SwapchainBuffer> swapchainBuffers;
        vk::DepthStencil depthStencil;
        std::vector<VkCommandBuffer> renderCmdBuffers;
        std::vector<VkFramebuffer> frameBuffers;
        uint32_t currentBuffer = 0;

        void initSwapchain(VkSurfaceKHR surface, bool vsync);
        void initFrameBuffers();
        void buildCommandBuffers();
    };
}

#endif
