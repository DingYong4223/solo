#pragma once

#include "SoloRenderer.h"


namespace solo
{
    class VulkanRenderer final: public Renderer
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        auto createTexture() -> TextureHandle override final;
        void destroyTexture(const TextureHandle& handle) override final;
        void set2DTexture(const TextureHandle& handle) override final;
        void set2DTexture(const TextureHandle& handle, uint32_t flags) override final;
        void set2DTexture(const TextureHandle& handle, uint32_t flags, float anisotropyLevel) override final;
        void setCubeTexture(const TextureHandle& handle) override final;
        void setCubeTexture(const TextureHandle& handle, uint32_t flags) override final;
        void setCubeTexture(const TextureHandle& handle, uint32_t flags, float anisotropyLevel) override final;
        void update2DTexture(const TextureHandle& handle, TextureFormat format, uint32_t width, uint32_t height,
            const void* data) override final;
        void updateCubeTexture(const TextureHandle& handle, CubeTextureFace face, TextureFormat format,
            uint32_t width, uint32_t height, const void* data) override final;
        void generateRectTextureMipmaps(const TextureHandle& handle) override final;
        void generateCubeTextureMipmaps(const TextureHandle& handle) override final;

        auto createFrameBuffer() -> FrameBufferHandle override final;
        void destroyFrameBuffer(const FrameBufferHandle& handle) override final;
        void setFrameBuffer(const FrameBufferHandle& handle) override final;
        void updateFrameBuffer(const FrameBufferHandle& handle, const std::vector<TextureHandle>& attachmentHandles) override final;

        auto createVertexBuffer(const VertexBufferLayout& layout, const void* data, uint32_t vertexCount)
            -> VertexBufferHandle override final;
        auto createDynamicVertexBuffer(const VertexBufferLayout& layout, const void* data, uint32_t vertexCount)
            -> VertexBufferHandle override final;
        void updateDynamicVertexBuffer(const VertexBufferHandle& handle, const void* data, uint32_t offset, uint32_t vertexCount) override final;
        void destroyVertexBuffer(const VertexBufferHandle& handle) override final;

        auto createIndexBuffer(const void* data, uint32_t elementSize, uint32_t elementCount) -> IndexBufferHandle override final;
        void destroyIndexBuffer(const IndexBufferHandle& handle) override final;

        auto createProgram(const char* vsSrc, const char* fsSrc) -> ProgramHandle override final;
        void destroyProgram(const ProgramHandle& handle) override final;
        void setProgram(const ProgramHandle& handle) override final;

        auto createVertexObject(const VertexBufferHandle* bufferHandles, uint32_t bufferCount, ProgramHandle programHandle)
            -> VertexObjectHandle override final;
        void destroyVertexObject(const VertexObjectHandle& handle) override final;

        auto createUniform(const char* name, UniformType type, ProgramHandle program) -> UniformHandle override final;
        void destroyUniform(const UniformHandle& handle) override final;
        void setUniform(const UniformHandle& handle, const void* value, uint32_t count) override final;

        void setFaceCull(FaceCull cull) override final;

        void setPolygonMode(PolygonMode mode) override final;

        void setBlend(bool enabled) override final;
        void setBlendFactor(BlendFactor srcFactor, BlendFactor dstFactor) override final;

        void setDepthWrite(bool enabled) override final;
        void setDepthTest(bool enabled) override final;
        void setDepthFunction(DepthFunction func) override final;

        void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override final;

        void clear(bool color, bool depth, float r, float g, float b, float a) override final;

        void drawIndexedVertexObject(PrimitiveType primitiveType, const VertexObjectHandle& vertexObjectHandle,
            const IndexBufferHandle& indexBufferHandle) override final;
        void drawVertexObject(PrimitiveType primitiveType, const VertexObjectHandle& vertexObjectHandle, uint32_t vertexCount) override final;
    };
}