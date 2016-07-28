#pragma once

#include "SoloRenderer.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloVulkan.h"

namespace solo
{
    class Device;
    class SDLVulkanDevice;

    class VulkanRenderer final: public Renderer
    {
    public:
        explicit VulkanRenderer(Device* device);
        ~VulkanRenderer();

        auto createTexture() -> TextureHandle override final { return EmptyTextureHandle; }
        void destroyTexture(const TextureHandle& handle) override final {}
        void set2DTexture(const TextureHandle& handle) override final {}
        void set2DTexture(const TextureHandle& handle, uint32_t flags) override final {}
        void set2DTexture(const TextureHandle& handle, uint32_t flags, float anisotropyLevel) override final {}
        void setCubeTexture(const TextureHandle& handle) override final {}
        void setCubeTexture(const TextureHandle& handle, uint32_t flags) override final {}
        void setCubeTexture(const TextureHandle& handle, uint32_t flags, float anisotropyLevel) override final {}
        void update2DTexture(const TextureHandle& handle, TextureFormat format, uint32_t width, uint32_t height, const void* data) override final {}
        void updateCubeTexture(const TextureHandle& handle, CubeTextureFace face, TextureFormat format,
            uint32_t width, uint32_t height, const void* data) override final {}
        void generateRectTextureMipmaps(const TextureHandle& handle) override final {}
        void generateCubeTextureMipmaps(const TextureHandle& handle) override final {}

        auto createFrameBuffer() -> FrameBufferHandle override final { return EmptyFrameBufferHandle; }
        void destroyFrameBuffer(const FrameBufferHandle& handle) override final {}
        void setFrameBuffer(const FrameBufferHandle& handle) override final {}
        void updateFrameBuffer(const FrameBufferHandle& handle, const std::vector<TextureHandle>& attachmentHandles) override final {}

        auto createVertexBuffer(const VertexBufferLayout& layout, const void* data, uint32_t vertexCount)
            -> VertexBufferHandle override final { return EmptyVertexBufferHandle; }
        auto createDynamicVertexBuffer(const VertexBufferLayout& layout, const void* data, uint32_t vertexCount)
            -> VertexBufferHandle override final { return EmptyVertexBufferHandle; }
        void updateDynamicVertexBuffer(const VertexBufferHandle& handle, const void* data, uint32_t offset, uint32_t vertexCount) override final {}
        void destroyVertexBuffer(const VertexBufferHandle& handle) override final {}

        auto createIndexBuffer(const void* data, uint32_t elementSize, uint32_t elementCount)
            -> IndexBufferHandle override final { return EmptyIndexBufferHandle; }
        void destroyIndexBuffer(const IndexBufferHandle& handle) override final {}

        auto createProgram(const char* vsSrc, const char* fsSrc) -> ProgramHandle override final { return EmptyProgramHandle; }
        void destroyProgram(const ProgramHandle& handle) override final {}
        void setProgram(const ProgramHandle& handle) override final {}

        auto createVertexObject(const VertexBufferHandle* bufferHandles, uint32_t bufferCount, ProgramHandle programHandle)
            -> VertexObjectHandle override final { return EmptyVertexObjectHandle; }
        void destroyVertexObject(const VertexObjectHandle& handle) override final {}

        auto createUniform(const char* name, UniformType type, ProgramHandle programHandle)
            -> UniformHandle override final { return EmptyUniformHandle; }
        void destroyUniform(const UniformHandle& handle) override final {}
        void setUniform(const UniformHandle& handle, const void* value, uint32_t count) override final {}

        void setFaceCull(FaceCull cull) override final {}

        void setPolygonMode(PolygonMode mode) override final {}

        void setBlend(bool enabled) override final {}
        void setBlendFactor(BlendFactor srcFactor, BlendFactor dstFactor) override final {}

        void setDepthWrite(bool enabled) override final {}
        void setDepthTest(bool enabled) override final {}
        void setDepthFunction(DepthFunction func) override final {}

        void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override final {}

        void clear(bool color, bool depth, float r, float g, float b, float a) override final {}

        void drawIndexedVertexObject(PrimitiveType primitiveType, const VertexObjectHandle& vertexObjectHandle,
            const IndexBufferHandle& indexBufferHandle) override final {}
        void drawVertexObject(PrimitiveType primitiveType, const VertexObjectHandle& vertexObjectHandle, uint32_t vertexCount) override final {}

    private:
        void createDevice(uint32_t queueIndex);
        void detectPhysicalDevice(VkInstance instance);
        void createSemaphores();
        void createCommandPool(uint32_t queueIndex);
        void createSwapchain(VkSurfaceKHR surface);

        SDLVulkanDevice* device = nullptr;
        uint32_t canvasWidth = 0;
        uint32_t canvasHeight = 0;
        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice logicalDevice = nullptr;
        VkQueue queue = nullptr;
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        VkCommandPool commandPool = nullptr;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        VkSemaphore presentCompleteSem = nullptr;
        VkSemaphore renderCompleteSem = nullptr;
        VkSubmitInfo submitInfo;
        VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    };
}

#else
#   error Vulkan renderer is not supported on this platform
#endif