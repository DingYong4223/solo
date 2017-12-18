/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "SoloCommon.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloMaterial.h"
#include "SoloVulkan.h"
#include "SoloVulkanBuffer.h"
#include "SoloVulkanEffect.h"

namespace solo
{
    class Device;
    class Effect;
    class Camera;
    class VulkanRenderer;
    class VulkanTexture;
    class VulkanPipelineConfig;

    class VulkanMaterial final: public Material
    {
    public:
        struct UniformBufferItem
        {
            std::function<void(VulkanBuffer&, const Camera*, const Transform*)> write;
        };

        struct Sampler
        {
            u32 binding;
            sptr<VulkanTexture> texture;
        };

        static auto createFromPrefab(Device *device, MaterialPrefab prefab) -> sptr<VulkanMaterial>;

        explicit VulkanMaterial(sptr<Effect> effect);
        ~VulkanMaterial();

        auto getEffect() const -> sptr<Effect> override final { return effect; }

        void setFloatParameter(const str &name, float value) override final;
        void setVector2Parameter(const str &name, const Vector2 &value) override final;
        void setVector3Parameter(const str &name, const Vector3 &value) override final;
        void setVector4Parameter(const str &name, const Vector4 &value) override final;
        void setMatrixParameter(const str &name, const Matrix &value) override final;
        void setTextureParameter(const str &name, sptr<solo::Texture> value) override final;
            
        void bindParameter(const str &name, BindParameterSemantics semantics) override final;

        auto getSamplers() const -> umap<str, Sampler> const& { return samplers; }
        auto getBufferItems() const -> umap<str, umap<str, UniformBufferItem>> const& { return bufferItems; } // TODO rename
        auto getStateHash() const -> size_t;

        void configurePipeline(VulkanPipelineConfig &cfg);

    private:
        using ParameterWriteFunc = std::function<void(VulkanBuffer&, u32, u32, const Camera*, const Transform*)>;

        sptr<VulkanEffect> effect;
        umap<str, umap<str, UniformBufferItem>> bufferItems;
        umap<str, Sampler> samplers;
        u32 version = 0;

        void setUniformParameter(const str &name, ParameterWriteFunc write);
    };
}

#endif