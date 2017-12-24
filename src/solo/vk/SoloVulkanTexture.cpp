/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloVulkanTexture.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloDevice.h"
#include "SoloVulkanRenderer.h"
#include "SoloTextureData.h"

using namespace solo;

static auto toFilter(TextureFilter filter) -> VkFilter
{
    switch (filter)
    {
        case TextureFilter::Linear: return VK_FILTER_LINEAR;
        case TextureFilter::Nearest: return VK_FILTER_NEAREST;
        default:
            SL_PANIC("Unsupported filter");
            return VK_FILTER_LINEAR;
    }
}

static auto toMipmapMode(TextureMipFilter mipFilter) -> VkSamplerMipmapMode
{
    switch (mipFilter)
    {
        case TextureMipFilter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        case TextureMipFilter::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case TextureMipFilter::None: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        default:
            SL_PANIC("Unsupported mip filter");
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

static auto toAddressMode(TextureWrap wrap) -> VkSamplerAddressMode
{
    switch (wrap)
    {
        case TextureWrap::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureWrap::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case TextureWrap::MirrorRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case TextureWrap::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        default:
            SL_PANIC("Unsupported wrap mode");
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

static auto createSampler(
    VkDevice device, VkPhysicalDeviceFeatures physicalFeatures, VkPhysicalDeviceProperties physicalProps,
    TextureFilter minFilter, TextureFilter magFilter, TextureMipFilter mipFilter, float mipLevels,
    TextureWrap horizontalWrap, TextureWrap verticalWrap, TextureWrap depthWrap,
    bool anisotropic, float anisotropyLevel) -> VulkanResource<VkSampler>
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.flags = 0;
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.maxAnisotropy = (std::min)(anisotropyLevel, physicalProps.limits.maxSamplerAnisotropy);
    samplerInfo.anisotropyEnable = physicalFeatures.samplerAnisotropy && anisotropic;
    samplerInfo.magFilter = toFilter(magFilter);
    samplerInfo.minFilter = toFilter(minFilter);
    samplerInfo.mipmapMode = toMipmapMode(mipFilter);
    samplerInfo.addressModeU = toAddressMode(horizontalWrap);
    samplerInfo.addressModeV = toAddressMode(verticalWrap);
    samplerInfo.addressModeW = toAddressMode(depthWrap);
    samplerInfo.mipLodBias = 0;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = mipFilter != TextureMipFilter::None ? mipLevels : 0;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

    VulkanResource<VkSampler> sampler{device, vkDestroySampler};
    SL_VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, sampler.cleanRef()));

    return sampler;
}

VulkanTexture::VulkanTexture(Device *device):
    renderer(static_cast<VulkanRenderer*>(device->getRenderer()))
{
}

VulkanTexture2d::VulkanTexture2d(Device *device, Texture2dData *data, bool generateMipmaps):
    Texture2d(data),
    VulkanTexture(device)
{
    const auto renderer = static_cast<VulkanRenderer*>(device->getRenderer());
    image = VulkanImage::create2d(renderer, data, generateMipmaps);
    rebuildSampler();
}

void VulkanTexture2d::rebuild()
{
    Texture2d::rebuild();
    rebuildSampler();
}

void VulkanTexture2d::rebuildSampler()
{
    sampler = createSampler(
        renderer->getDevice(),
        renderer->getPhysicalFeatures(),
        renderer->getPhysicalProperties(),
        minFilter, magFilter, mipFilter, image.getMipLevels(),
        horizontalWrap, verticalWrap, TextureWrap::Repeat,
        anisotropyLevel > 1, anisotropyLevel);
}

VulkanCubeTexture::VulkanCubeTexture(Device *device, CubeTextureData *data):
    CubeTexture(data),
    VulkanTexture(device)
{
    const auto renderer = static_cast<VulkanRenderer*>(device->getRenderer());
    image = VulkanImage::createCube(renderer, data);
    rebuildSampler();
}

void VulkanCubeTexture::rebuild()
{
    CubeTexture::rebuild();
    rebuildSampler();
}

void VulkanCubeTexture::rebuildSampler()
{
    sampler = createSampler(
        renderer->getDevice(),
        renderer->getPhysicalFeatures(),
        renderer->getPhysicalProperties(),
        minFilter, magFilter, mipFilter, image.getMipLevels(),
        horizontalWrap, verticalWrap, depthWrap,
        anisotropyLevel > 1, anisotropyLevel);
}

#endif
