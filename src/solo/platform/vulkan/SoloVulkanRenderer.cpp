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

#include "SoloVulkanRenderer.h"
#include "SoloSDLVulkanDevice.h"
#include "SoloVulkanDescriptorSetLayoutBuilder.h"
#include "SoloVulkanDescriptorPool.h"
#include "SoloVulkanPipeline.h"
#include "SoloVulkanBuffer.h"
// TODO remove?
#include "SoloVector3.h"
#include "SoloVector4.h"
#include "SoloFileSystem.h"


#ifdef SL_VULKAN_RENDERER

using namespace solo;

// TODO remove
Device *engineDevice = nullptr;


VulkanRenderer::VulkanRenderer(Device *engineDevice):
    canvasWidth(engineDevice->getSetup().canvasWidth),
    canvasHeight(engineDevice->getSetup().canvasHeight)
{
    ::engineDevice = engineDevice; // TODO remove

    auto vulkanDevice = dynamic_cast<SDLVulkanDevice*>(engineDevice);
    auto instance = vulkanDevice->getVkInstance();
    auto surface = vulkanDevice->getVkSurface();

    physicalDevice = vk::getPhysicalDevice(instance);
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    auto surfaceFormats = vk::getSurfaceFormats(physicalDevice, surface);
    colorFormat = std::get<0>(surfaceFormats);
    colorSpace = std::get<1>(surfaceFormats);

    auto queueIndex = vk::getQueueIndex(physicalDevice, surface);

    device = vk::createDevice(physicalDevice, queueIndex);

    vkGetDeviceQueue(device, queueIndex, 0, &queue);

    depthFormat = vk::getDepthFormat(physicalDevice);
    presentCompleteSem = vk::createSemaphore(device);
    renderCompleteSem = vk::createSemaphore(device);
    commandPool = vk::createCommandPool(device, queueIndex);
    depthStencil = vk::createDepthStencil(device, memProperties, depthFormat, canvasWidth, canvasHeight);
    renderPass = vk::createRenderPass(device, colorFormat, depthFormat);
    
    initSwapchain(surface, engineDevice->getSetup().vsync);
    initFrameBuffers();
    
    renderCmdBuffers.resize(swapchainBuffers.size());
    vk::createCommandBuffers(device, commandPool, swapchainBuffers.size(), renderCmdBuffers.data());
}


VulkanRenderer::~VulkanRenderer()
{
    vkFreeCommandBuffers(device, commandPool, renderCmdBuffers.size(), renderCmdBuffers.data());

    for (size_t i = 0; i < frameBuffers.size(); ++i)
        vkDestroyFramebuffer(device, frameBuffers[i], nullptr);

    for (auto &buf : swapchainBuffers)
        vkDestroyImageView(device, buf.imageView, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    // Render pass
    vkDestroyRenderPass(device, renderPass, nullptr);

    // Depth stencil
    if (depthStencil.view)
        vkDestroyImageView(device, depthStencil.view, nullptr);
    if (depthStencil.image)
        vkDestroyImage(device, depthStencil.image, nullptr);
    if (depthStencil.mem)
        vkFreeMemory(device, depthStencil.mem, nullptr);

    // Command pool
    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroySemaphore(device, presentCompleteSem, nullptr);
    vkDestroySemaphore(device, renderCompleteSem, nullptr);

    vkDestroyDevice(device, nullptr);
}


// TODO remove
bool initialized = false;


void VulkanRenderer::
beginFrame()
{
    // TODO remove
    if (!initialized)
    {
        initTest(engineDevice);
        initialized = true;
    }

    SL_CHECK_VK_RESULT(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, presentCompleteSem, nullptr, &currentBuffer));
}


void VulkanRenderer::endFrame()
{
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = &submitPipelineStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &presentCompleteSem;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderCompleteSem;
    submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &renderCmdBuffers[currentBuffer];
    SL_CHECK_VK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &currentBuffer;
	presentInfo.pWaitSemaphores = &renderCompleteSem;
	presentInfo.waitSemaphoreCount = 1;
    SL_CHECK_VK_RESULT(vkQueuePresentKHR(queue, &presentInfo));

    SL_CHECK_VK_RESULT(vkQueueWaitIdle(queue));
}


void VulkanRenderer::initSwapchain(VkSurfaceKHR surface, bool vsync)
{
    VkSurfaceCapabilitiesKHR capabilities;
    SL_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

    uint32_t presentModeCount;
    SL_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    SL_CHECK_VK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));

    auto width = static_cast<uint32_t>(this->canvasWidth);
    auto height = static_cast<uint32_t>(this->canvasHeight);
    if (capabilities.currentExtent.width != static_cast<uint32_t>(-1))
    {
        width = capabilities.currentExtent.width;
        height = capabilities.currentExtent.height;
    }

    auto presentMode = VK_PRESENT_MODE_FIFO_KHR; // "vsync"

    if (!vsync)
    {
        for (const auto mode : presentModes)
        {
            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                presentMode = mode;
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = mode;
                break;
            }
        }
    }

    VkSurfaceTransformFlagsKHR transformFlags;
    if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        transformFlags = capabilities.currentTransform;

    auto requestedImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && requestedImageCount > capabilities.maxImageCount)
        requestedImageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = requestedImageCount;
    swapchainInfo.imageFormat = colorFormat;
    swapchainInfo.imageColorSpace = colorSpace;
    swapchainInfo.imageExtent = {width, height};
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(transformFlags);
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.oldSwapchain = nullptr; // TODO
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    SL_CHECK_VK_RESULT(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain));

    uint32_t imageCount = 0;
    SL_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));

    std::vector<VkImage> images;
    images.resize(imageCount);
    SL_CHECK_VK_RESULT(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()));

    swapchainBuffers.resize(imageCount);

    for (uint32_t i = 0; i < imageCount; i++)
    {
        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.pNext = nullptr;
        imageViewInfo.format = colorFormat;
        imageViewInfo.components =
        {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A
        };
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.flags = 0;
        imageViewInfo.image = images[i];

        swapchainBuffers[i].image = images[i];

        SL_CHECK_VK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &swapchainBuffers[i].imageView));
    }
}


void VulkanRenderer::initFrameBuffers()
{
    auto count = swapchainBuffers.size();
    frameBuffers.resize(count);
    for (auto i = 0; i < count; i++)
        frameBuffers[i] = vk::createFrameBuffer(device, swapchainBuffers[i].imageView, depthStencil.view, renderPass, canvasWidth, canvasHeight);
}


// TODO Remove after testing
struct Vertex
{
    Vector2 position;
    Vector3 color;
};

VulkanDescriptorPool descriptorPool;
VkDescriptorSetLayout descSetLayout;
VulkanBuffer vertexBuffer;
VulkanBuffer uniformBuffer;
VkPipeline pipeline = nullptr;


void VulkanRenderer::initTest(Device *engineDevice)
{
    VulkanDescriptorSetLayoutBuilder builder(device);
    builder.setBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS);
    descSetLayout = builder.build();

    descriptorPool = VulkanDescriptorPool(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, 1);
    auto descriptorSet = descriptorPool.allocateSet(descSetLayout);

    auto vertexShader = vk::createShader(device, engineDevice->getFileSystem()->readBytes("../assets/triangle.vert.spv"));
    auto fragmentShader = vk::createShader(device, engineDevice->getFileSystem()->readBytes("../assets/triangle.frag.spv"));

    VulkanPipeline pipeline(device, renderPass);
    pipeline.setVertexShader(vertexShader, "main");
    pipeline.setFragmentShader(fragmentShader, "main");

    pipeline.setDescriptorSetLayouts(&descSetLayout, 1);

    pipeline.setVertexSize(sizeof(Vertex));

    pipeline.setVertexAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position));
    pipeline.setVertexAttribute(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, color));

    pipeline.setTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    pipeline.setCullMode(VK_CULL_MODE_BACK_BIT);
    pipeline.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    pipeline.setRasterizationSampleCount(VK_SAMPLE_COUNT_1_BIT);
    pipeline.setColorBlend(VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    pipeline.setAlphaBlend(VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
    pipeline.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

    pipeline.rebuild();

    const auto triangleSize = 1.6f;
    std::vector<Vertex> triangle1 =
    {
		{ Vector2(0.5f * triangleSize, sqrtf(3.0f) * 0.25f * triangleSize), Vector3(1.0f, 0.0f, 0.0f) },
		{ Vector2(0.0f, -sqrtf(3.0f) * 0.25f * triangleSize), Vector3(0.0f, 1.0f, 0.0f) },
		{ Vector2(-0.5f * triangleSize, sqrtf(3.0f) * 0.25f * triangleSize), Vector3(0.0f, 0.0f, 1.0f) }
	};

    std::vector<Vertex> triangle2 =
    {
		{ Vector2(0.3f * triangleSize, sqrtf(3.0f) * 0.25f * triangleSize), Vector3(1.0f, 0.0f, 0.0f) },
		{ Vector2(0.0f, -sqrtf(3.0f) * 0.25f * triangleSize), Vector3(0.0f, 1.0f, 0.0f) },
		{ Vector2(-0.5f * triangleSize, sqrtf(3.0f) * 0.25f * triangleSize), Vector3(0.0f, 0.0f, 1.0f) }
	};

    auto stagingBuffer = VulkanBuffer(device, sizeof(decltype(triangle1)::value_type) * triangle1.size(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memProperties);
    stagingBuffer.update(triangle2.data());
    stagingBuffer.update(triangle1.data());
    stagingBuffer.update(triangle2.data()); // just smoking

    vertexBuffer = VulkanBuffer(device, sizeof(decltype(triangle1)::value_type) * triangle1.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        memProperties);
    stagingBuffer.transferTo(vertexBuffer, queue, commandPool);

    auto uniformColor = Vector4(0, 0.2f, 0.8f, 1);
    uniformBuffer = VulkanBuffer(device, sizeof(Vector4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memProperties);
    uniformBuffer.update(&uniformColor);

    VkDescriptorBufferInfo uboInfo {};
    uboInfo.buffer = uniformBuffer.getHandle();
    uboInfo.offset = 0;
    uboInfo.range = sizeof(Vector3);

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &uboInfo;
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

    recordCommandBuffers([&](VkCommandBuffer buf)
    {
        pipeline.bind(buf);

        VkDeviceSize offset = 0;
        auto& buffer = vertexBuffer.getHandle();
	    vkCmdBindVertexBuffers(buf, 0, 1, &buffer, &offset);

        vkCmdBindDescriptorSets(buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getLayoutHandle(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDraw(buf, static_cast<uint32_t>(triangle1.size()), 1, 0, 0);
    });
}


void VulkanRenderer::renderTest()
{
}


void VulkanRenderer::recordCommandBuffers(std::function<void(VkCommandBuffer)> commands)
{
    static VkClearColorValue defaultClearColor = {{0.5f, 0.1f, 0.1f, 1.0f}};

    // Build

    VkViewport viewport {};
	viewport.width = canvasWidth;
	viewport.height = canvasHeight;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

    VkRect2D scissor {};
    scissor.extent.width = canvasWidth;
    scissor.extent.height = canvasHeight;
    scissor.offset.x = 0;
    scissor.offset.y = 0;

    VkClearValue clearValues[2];
	clearValues[0].color = defaultClearColor;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = canvasWidth;
	renderPassBeginInfo.renderArea.extent.height = canvasHeight;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

    // TODO this is only for testing
    for (size_t i = 0; i < renderCmdBuffers.size(); i++)
    {
        renderPassBeginInfo.framebuffer = frameBuffers[i];

        vk::recordCommandBuffer(renderCmdBuffers[i], [=](VkCommandBuffer buf)
        {
            vkCmdBeginRenderPass(buf, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdSetViewport(renderCmdBuffers[i], 0, 1, &viewport);
            vkCmdSetScissor(renderCmdBuffers[i], 0, 1, &scissor);

            commands(buf);

            vkCmdEndRenderPass(buf);
        });
    }
}

#endif
