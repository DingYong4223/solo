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

#include "SoloVulkanDescriptorSetLayoutBuilder.h"

#ifdef SL_VULKAN_RENDERER

using namespace solo;
using namespace vk;


DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(VkDevice device):
    device(device)
{
}


auto DescriptorSetLayoutBuilder::withBinding(uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
    VkShaderStageFlagBits stageFlags) -> DescriptorSetLayoutBuilder&
{
    if (binding >= bindings.size())
        bindings.resize(binding + 1);
    bindings[binding].binding = binding;
    bindings[binding].descriptorType = descriptorType;
    bindings[binding].descriptorCount = descriptorCount;
    bindings[binding].stageFlags = stageFlags;
    bindings[binding].pImmutableSamplers = nullptr;

    return *this;
}


auto DescriptorSetLayoutBuilder::build() -> Resource<VkDescriptorSetLayout>
{
    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    Resource<VkDescriptorSetLayout> result{device, vkDestroyDescriptorSetLayout};
    SL_VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, result.cleanAndExpose()));
    
    return result;
}


#endif