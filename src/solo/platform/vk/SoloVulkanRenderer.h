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

#include "SoloCommon.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloRenderer.h"
#include "SoloVulkanSwapchain.h"
#include "SoloVulkanRenderPass.h"
#include "SoloVulkanDescriptorPool.h"
#include "SoloVulkanBuffer.h"
#include "SoloVulkanPipeline.h"
#include "SoloVulkan.h"
#include "SoloVector4.h"

namespace solo
{
    class Device;

    namespace vk
    {
        class Renderer final : public solo::Renderer
        {
        public:
            explicit Renderer(Device *device);
            ~Renderer();

            void beginRenderPass(uint32_t canvasWidth, uint32_t canvasHeight,
                bool clearColor, bool clearDepth, const Vector4 &color);
            void endRenderPass();

            void setViewport(const Vector4 &viewport);

        protected:
            void beginFrame() override final;
            void endFrame() override final;

        private:
            VkFormat depthFormat = VK_FORMAT_UNDEFINED;
            VkFormat colorFormat = VK_FORMAT_UNDEFINED;
            VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

            struct
            {
                VkPhysicalDevice device = nullptr;
                VkPhysicalDeviceFeatures features;
                VkPhysicalDeviceProperties properties;
                VkPhysicalDeviceMemoryProperties memProperties;
            } physicalDevice;

            Resource<VkDevice> device;
            VkQueue queue = nullptr;
            Resource<VkCommandPool> commandPool;
            DepthStencil depthStencil;
            RenderPass renderPass;
            Swapchain swapchain;

            struct
            {
                Resource<VkSemaphore> renderComplete;
                Resource<VkSemaphore> presentComplete;
            } semaphores;

            std::vector<VkCommandBuffer> cmdBuffers;
            uint32_t currentBuffer = 0;

            bool dirty = true;

            enum class RenderCommandType
            {
                None,
                BeginRenderPass,
                EndRenderPass,
                SetViewport
            };

            struct RenderCommand
            {
                RenderCommandType type = RenderCommandType::None;

                union
                {
                    struct
                    {
                        uint32_t canvasWidth;
                        uint32_t canvasHeight;
                        Vector4 color;
                        bool clearColor;
                        bool clearDepth;
                    } beginRenderPass;

                    VkViewport viewport;
                };

                RenderCommand() {}
                RenderCommand(RenderCommandType type): type(type) {}
            };

            // TODO currently we just always recreate render plan.
            // In the future these command sets should be compared to see it something
            // really needs to be done
            std::vector<RenderCommand> renderCommands;
            std::vector<RenderCommand> prevRenderCommands;

            struct
            {
                DescriptorPool descriptorPool;
                Resource<VkDescriptorSetLayout> descSetLayout;
                Buffer vertexBuffer;
                Buffer uniformBuffer;
                Pipeline pipeline;
            } test;

            void updateCmdBuffers();
        };
    }
}

#endif
