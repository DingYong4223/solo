/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloVulkanMaterial.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloEffect.h"
#include "SoloDevice.h"
#include "SoloVulkanRenderer.h"
#include "SoloVulkanDescriptorSetLayoutBuilder.h"
#include "SoloVulkanDescriptorSetUpdater.h"

using namespace solo;

static auto parseIndex(const char *from, uint32_t len) -> int32_t
{
    int32_t index = 0;
    int digit = 1;
    while (len > 0)
    {
        index += digit * (*(from + len - 1) - '0');
        digit *= 10;
        len--;
    }

    return index;
}

static auto parseName(const std::string &name) -> std::tuple<uint32_t, int32_t>
{
    const auto idx = name.find(":");
    const auto rawName = name.c_str();
    const uint32_t binding = parseIndex(rawName, idx == std::string::npos ? name.size() : idx);
    const int32_t indexInBuffer = idx == std::string::npos ? -1 : parseIndex(rawName + idx + 1, name.size() - idx - 1);
    return std::make_tuple(binding, indexInBuffer);
}

vk::Material::Material(sptr<Effect> effect):
    solo::Material(effect)
{
}

vk::Material::~Material()
{
}

void vk::Material::setFloatParameter(const std::string &name, float value)
{
    auto parsedName = parseName(name);
    const int32_t bindingIndex = std::get<0>(parsedName);
    int32_t bufferItemIndex = std::get<1>(parsedName);

    if (bindings.count(bindingIndex) == 0)
    {
        bindings[bindingIndex];
        dirtyLayout = true;
    }

    auto &binding = bindings.at(bindingIndex);
    SL_PANIC_IF(bufferItemIndex >= 0 && binding.texture, "Parameter already has different type");
    SL_PANIC_IF(
        bufferItemIndex >= 0 &&
        binding.bufferItems.size() > bufferItemIndex &&
        binding.bufferItems[bufferItemIndex].size != sizeof(float),
        "Parameter already has different size"
    );

    if (binding.bufferItems.size() < bufferItemIndex + 1)
    {
        binding.bufferItems.resize(bufferItemIndex + 1);
        dirtyLayout = true;
    }

    auto &item = binding.bufferItems[bufferItemIndex];
    item.size = sizeof(float);
    item.write = [value](Buffer &buffer, uint32_t offset)
    {
        buffer.updatePart(&value, offset, sizeof(float));
    };

    binding.dirtyData = true;
}

void vk::Material::setFloatArrayParameter(const std::string &name, const std::vector<float> &value)
{
}

void vk::Material::setVector2Parameter(const std::string &name, const Vector2 &value)
{
}

void vk::Material::setVector2ArrayParameter(const std::string &name, const std::vector<Vector2> &value)
{
}

void vk::Material::setVector3Parameter(const std::string &name, const Vector3 &value)
{
}

void vk::Material::setVector3ArrayParameter(const std::string &name, const std::vector<Vector3> &value)
{
}

void vk::Material::setVector4Parameter(const std::string &name, const Vector4 &value)
{
}

void vk::Material::setVector4ArrayParameter(const std::string &name, const std::vector<Vector4> &value)
{
}

void vk::Material::setMatrixParameter(const std::string &name, const Matrix &value)
{
}

void vk::Material::setMatrixArrayParameter(const std::string &name, const std::vector<Matrix> &value)
{
}

void vk::Material::setTextureParameter(const std::string &name, sptr<Texture> value)
{
}

void vk::Material::bindWorldMatrixParameter(const std::string &name)
{
}

void vk::Material::bindViewMatrixParameter(const std::string &name)
{
}

void vk::Material::bindProjectionMatrixParameter(const std::string &name)
{
}

void vk::Material::bindWorldViewMatrixParameter(const std::string &name)
{
}

void vk::Material::bindViewProjectionMatrixParameter(const std::string &name)
{
}

void vk::Material::bindWorldViewProjectionMatrixParameter(const std::string &name)
{
}

void vk::Material::bindInvTransposedWorldMatrixParameter(const std::string &name)
{
}

void vk::Material::bindInvTransposedWorldViewMatrixParameter(const std::string &name)
{
}

void vk::Material::bindCameraWorldPositionParameter(const std::string &name)
{
}

void vk::Material::applyParameters(Renderer *renderer)
{
    // Store everything in one set for simplicity
    // "0:2" means "binding 0, index 2 in the uniform buffer" (for uniforms)
    // "1" means "set 0, sampler binding 1"

    if (dirtyLayout)
    {
        auto builder = vk::DescriptorSetLayoutBuilder(renderer->getDevice());

        auto samplerCount = 0;
        auto uniformBufferCount = 0;
        for (auto &b: bindings)
        {
            if (b.second.texture)
            {
                builder.withBinding(b.first, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
                samplerCount++;
            }
            else if (!b.second.bufferItems.empty())
            {
                builder.withBinding(b.first, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS);
                uniformBufferCount++;

                b.second.bufferSize = 0;
                for (const auto &item: b.second.bufferItems)
                    b.second.bufferSize += item.size;
                // TODO Make sure the old buffer is destroyed
                b.second.buffer = Buffer::createUniformHostVisible(renderer, b.second.bufferSize);
            }
        }

        descSetLayout = builder.build();

        auto poolConfig = vk::DescriptorPoolConfig();
        if (uniformBufferCount > 0)
            poolConfig.forDescriptors(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBufferCount);
        if (samplerCount > 0)
            poolConfig.forDescriptors(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, samplerCount);

        // TODO Make sure this destroys the old pool
        descPool = vk::DescriptorPool(renderer->getDevice(), 1, poolConfig);

        descSet = descPool.allocateSet(descSetLayout);

        vk::DescriptorSetUpdater updater{renderer->getDevice()};

        for (auto &b : bindings)
        {
            if (b.second.buffer) // TODO how's this buffer-to-bool comparison supposed to work?
                updater.forUniformBuffer(b.first, descSet, b.second.buffer, 0, b.second.buffer.getSize());
            else if (b.second.texture)
                updater.forTexture(b.first, descSet, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        updater.updateSets();

        dirtyLayout = false;
    }

    for (auto &b : bindings)
    {
        if (!b.second.dirtyData || !b.second.buffer) // TODO how's this buffer-to-bool comparison supposed to work?
            continue;

        uint32_t offset = 0;
        for (const auto &item : b.second.bufferItems)
        {
            if (item.size > 0 && item.write) // check against accidental gaps in items array
                item.write(b.second.buffer, offset);
            offset += item.size;
        }

        b.second.dirtyData = false;
    }
}

#endif
