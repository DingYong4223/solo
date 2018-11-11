/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#pragma once

#include "SoloCommon.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloVulkanDescriptorSet.h"
#include "SoloVulkanBuffer.h"
#include "SoloVulkan.h"
#include "SoloVector4.h"

namespace solo
{
    class VulkanRenderPass;
    class VulkanRenderer;

    class VulkanCmdBuffer
    {
    public:
        VulkanCmdBuffer() = default;
        VulkanCmdBuffer(VulkanRenderer *renderer);
        VulkanCmdBuffer(const VulkanCmdBuffer &other) = delete;
        VulkanCmdBuffer(VulkanCmdBuffer &&other) = default;
        ~VulkanCmdBuffer() = default;

        void flush();

        void begin(bool oneTime); // TODO avoid oneTime param
        void end();

        void beginRenderPass(const VulkanRenderPass &pass, VkFramebuffer framebuffer, u32 canvasWidth, u32 canvasHeight);
        void endRenderPass();

        void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);
        void bindVertexBuffer(u32 binding, VkBuffer buffer);
        void drawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance);
        void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

        void bindPipeline(VkPipeline pipeline);
        void bindDescriptorSet(VkPipelineLayout pipelineLayout, const VulkanDescriptorSet &set);

        void setViewport(const Vector4 &dimentions, float minDepth, float maxDepth);
        void setScissor(const Vector4 &dimentions);

        void clearColorAttachment(u32 attachment, const VkClearValue &clearValue, const VkClearRect &clearRect);

        void copyBuffer(const VulkanBuffer &src, const VulkanBuffer &dst);

        auto operator=(const VulkanCmdBuffer &other) -> VulkanCmdBuffer& = delete;
        auto operator=(VulkanCmdBuffer &&other) -> VulkanCmdBuffer& = default;

        operator bool() const { return handle_; }
        operator const VkCommandBuffer*() { return &handle_; }

    private:
        VulkanRenderer *renderer_ = nullptr;
        VulkanResource<VkCommandBuffer> handle_;
    };
}

#endif