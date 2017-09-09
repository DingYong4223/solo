/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "SoloVulkanMaterial.h"

#ifdef SL_VULKAN_RENDERER

#include "SoloDevice.h"
#include "SoloVulkanEffect.h"
#include "SoloVulkanRenderer.h"
#include "SoloVulkanDescriptorSetLayoutBuilder.h"
#include "SoloVulkanDescriptorSetUpdater.h"
#include "SoloVulkanTexture.h"

using namespace solo;

static auto parseName(const std::string &name) -> std::tuple<std::string, std::string>
{
    const auto idx = name.find(".");
    const auto first = (idx != std::string::npos) ? name.substr(0, idx) : name;
    const auto second = (idx != std::string::npos) ? name.substr(idx + 1) : "";
    return std::make_tuple(first, second);
}

vk::Material::Material(sptr<solo::Effect> effect):
    solo::Material(effect),
    vkEffect(std::static_pointer_cast<vk::Effect>(effect))
{
}

vk::Material::~Material()
{
}

auto vk::Material::getCullModeFlags() const -> VkCullModeFlags
{
    switch (faceCull)
    {
        case FaceCull::All: return VK_CULL_MODE_NONE;
        case FaceCull::CCW: return VK_CULL_MODE_FRONT_BIT;
        case FaceCull::CW: return VK_CULL_MODE_BACK_BIT;
        default:
            SL_PANIC("Unsupported face cull mode");
            return VK_FRONT_FACE_CLOCKWISE;
    }
}

auto vk::Material::getPolygonMode() const -> VkPolygonMode
{
    switch (polygonMode)
    {
        case PolygonMode::Points: return VK_POLYGON_MODE_POINT;
        case PolygonMode::Triangle: return VK_POLYGON_MODE_FILL;
        case PolygonMode::Wireframe: return VK_POLYGON_MODE_LINE;
        default:
            SL_PANIC("Unsupported polygon mode");
            return VK_POLYGON_MODE_FILL;
    }
}

#define IMPLEMENT_SET_UNIFORM_PARAM_METHOD(methodName, paramType) \
void vk::Material::methodName(const std::string &name, paramType value) \
{ \
    auto parsedName = parseName(name); \
    auto bufferName = std::get<0>(parsedName); \
    auto fieldName = std::get<1>(parsedName); \
    SL_PANIC_IF(bufferName.empty() || fieldName.empty(), SL_FMT("Invalid parameter name ", name)); \
\
    auto bufferInfo = vkEffect->getUniformBufferInfo(bufferName); \
    SL_PANIC_IF(!bufferInfo.members.count(fieldName), SL_FMT("Unknown buffer ", bufferName, " or buffer member ", fieldName)); \
    auto itemInfo = bufferInfo.members.at(fieldName); \
\
    auto &buffer = uniformBuffers[bufferName]; \
    buffer.dirty = true; \
    buffer.binding = bufferInfo.binding; \
    buffer.size = bufferInfo.size; \
    if (!buffer.buffer) \
        dirtyLayout = true; \
\
    auto &item = buffer.items[fieldName]; \
    item.dirty = true; \
    item.write = [value, itemInfo](Buffer &buffer) \
    { \
        buffer.updatePart(&value, itemInfo.offset, itemInfo.size); \
    };\
}

IMPLEMENT_SET_UNIFORM_PARAM_METHOD(setFloatParameter, float)
IMPLEMENT_SET_UNIFORM_PARAM_METHOD(setVector2Parameter, const Vector2&)
IMPLEMENT_SET_UNIFORM_PARAM_METHOD(setVector3Parameter, const Vector3&)
IMPLEMENT_SET_UNIFORM_PARAM_METHOD(setVector4Parameter, const Vector4&)
IMPLEMENT_SET_UNIFORM_PARAM_METHOD(setMatrixParameter, const Matrix&)

void vk::Material::setTextureParameter(const std::string &name, sptr<solo::Texture> value)
{
    const auto samplerInfo = vkEffect->getSamplerInfo(name);
    auto &sampler = samplers[name];
    sampler.binding = samplerInfo.binding;
    sampler.texture = std::dynamic_pointer_cast<vk::Texture>(value);
    dirtyLayout = true;
    // TODO Optimize and mark only this sampler as dirty
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
    if (dirtyLayout)
    {
        auto builder = vk::DescriptorSetLayoutBuilder(renderer->getDevice());

        for (auto &pair : uniformBuffers)
        {
            auto &info = pair.second;
            if (!info.buffer)
            {
                builder.withBinding(info.binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS);
                // TODO Make sure this destroys the old buffer
                info.buffer = Buffer::createUniformHostVisible(renderer, info.size);
            }
        }

        for (auto &pair : samplers)
        {
            auto &info = pair.second;
            builder.withBinding(info.binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
                VK_SHADER_STAGE_FRAGMENT_BIT);
        }

        descSetLayout = builder.build();

        auto poolConfig = vk::DescriptorPoolConfig();
        if (!uniformBuffers.empty())
            poolConfig.forDescriptors(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uniformBuffers.size());
        if (!samplers.empty())
            poolConfig.forDescriptors(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, samplers.size());

        // TODO Make sure this destroys the old pool
        descPool = vk::DescriptorPool(renderer->getDevice(), 1, poolConfig);
        descSet = descPool.allocateSet(descSetLayout);

        vk::DescriptorSetUpdater updater{renderer->getDevice()};

        for (auto &pair : uniformBuffers)
        {
            auto &info = pair.second;
            updater.forUniformBuffer(info.binding, descSet, info.buffer, 0, info.size);
        }

        for (auto &pair : samplers)
        {
            auto &info = pair.second;
            updater.forTexture(info.binding, descSet, info.texture->getView(), info.texture->getSampler(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        updater.updateSets();

        dirtyLayout = false;
    }

    for (auto &pair : uniformBuffers)
    {
        auto &info = pair.second;
        if (info.dirty)
        {
            for (auto &p : info.items)
            {
                if (p.second.dirty)
                {
                    p.second.write(info.buffer);
                    p.second.dirty = false;
                }
            }
        }
    }
}

#endif
