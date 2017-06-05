#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include "magma.h"
#include "logger.h"

#define VK_VER_MAJOR(X)   ((((uint32_t)(X)) >> 22) & 0x3FF)
#define VK_VER_MINOR(X)   ((((uint32_t)(X)) >> 12) & 0x3FF)
#define VK_VER_PATCH(X)   (((uint32_t) (X))        & 0xFFF)
#define VK_CHECK(result)  if (result != VK_SUCCESS) { \
  logger->error("Vulkan API called failed with error %d", result); abort(); \
}

namespace VkWrap {
  inline std::vector<const char*> getAvailableWSIExtensions() {
    std::vector<const char*> extensions;
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
    return extensions;
  }
  inline VkResult createInstance(VkInstance& instance) {
    // Use validation layers if this is a debug build, and use WSI extensions regardless
    std::vector<const char*> extensions = getAvailableWSIExtensions();
    std::vector<const char*> layers;
#if defined(_DEBUG)
    layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = MAGMA_DISPLAY_NAME;
    appInfo.applicationVersion = MAGMA_VERSION;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
    
    return vkCreateInstance(&instanceInfo, nullptr, &instance);
  }
  inline VkResult getDevices(const VkInstance& instance, std::vector<VkPhysicalDevice>& deviceVec) {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    deviceVec.resize(deviceCount);
    return vkEnumeratePhysicalDevices(instance, &deviceCount, deviceVec.data());
  }
  inline void getQueueFamilyProperties(const VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& queueFamilyPropsVec) {
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    queueFamilyPropsVec.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilyPropsVec.data());
  }
  inline int32_t getQueueFamilyIndex(std::vector<VkQueueFamilyProperties>& queueFamilyPropsVec, const int32_t queueFamilyFlags) {
    for (uint32_t i = 0; i < queueFamilyPropsVec.size(); i++) {
      if (queueFamilyPropsVec[i].queueFlags & queueFamilyFlags) {
        return i;
      }
    }
    return -1;
  }
  inline VkResult createSemaphore(const VkDevice& device, VkSemaphore& semaphore) {
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;
    return vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, nullptr, &semaphore);
  }
  inline VkResult createFence(const VkDevice& device, VkFence& fence) {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    return vkCreateFence(device, &fenceInfo, nullptr, &fence);
  }
  inline VkResult createDevice(const VkPhysicalDevice& physicalDevice, VkDevice& device) {
    std::vector<VkQueueFamilyProperties> queueFamilyPropsVec;
    getQueueFamilyProperties(physicalDevice, queueFamilyPropsVec);
    int32_t queueFamilyIndex = getQueueFamilyIndex(queueFamilyPropsVec, VK_QUEUE_GRAPHICS_BIT);
    if (queueFamilyIndex < 0) {
      logger->error("No suitable device for performing graphics operations has been found.");
      return (VkResult)-1;
    }
    
    float queuePriorities[1] = { 0.f };
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;
    
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    return vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
  }
  inline void printPhysicalDevice(const VkPhysicalDevice& physicalDevice) {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    logger->info("Driver version {0}", physicalDeviceProperties.driverVersion);
    logger->info("Device name {0}", physicalDeviceProperties.deviceName);
    logger->info("Device type {0}", physicalDeviceProperties.deviceType);
    logger->info("API version {0:d}{0:d}{0:d}", VK_VER_MAJOR(physicalDeviceProperties.apiVersion),
                                                VK_VER_MINOR(physicalDeviceProperties.apiVersion),
                                                VK_VER_PATCH(physicalDeviceProperties.apiVersion));
  }
  inline VkResult createCommandPool(const VkDevice& device, int32_t queueFamilyIndex, VkCommandPool& commandPool) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = queueFamilyIndex;
    info.flags = 0;
    return vkCreateCommandPool(device, &info, nullptr, &commandPool);
  }
  inline VkResult createCommandBuffer(const VkDevice& device, const VkCommandPool& commandPool, VkCommandBuffer& commandBuffer) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;
    return vkAllocateCommandBuffers(device, &info, &commandBuffer);
  }
  inline VkResult createSurface(const VkInstance& instance, HWND hwnd, VkSurfaceKHR& surface) {
#if defined(SDL_VIDEO_DRIVER_WINDOWS) && defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(NULL);
    surfaceInfo.hwnd = hwnd;
    return vkCreateWin32SurfaceKHR(instance, &surfaceInfo, NULL, &surface);
#endif
    return VK_SUCCESS;
  }
  inline VkResult createSwapchain(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const VkSurfaceKHR& surface, VkFormat format, VkSwapchainKHR& swapchain) {
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.surface = surface;
    info.imageFormat = format;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    info.minImageCount = surfaceCapabilities.minImageCount;
    info.imageExtent = surfaceCapabilities.currentExtent;
    info.preTransform = surfaceCapabilities.currentTransform;
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    return vkCreateSwapchainKHR(device, &info, nullptr, &swapchain);
  }
  inline VkResult createSwapchainImageViews(const VkDevice& device, const VkSwapchainKHR& swapchain, VkFormat format, std::vector<VkImageView>& swapchainImageViews) {
    uint32_t imageCount;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    imageViews.resize(imageCount);
    if (vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()) != VK_SUCCESS) {
      logger->error("Swapchain images creation failed.");
      return (VkResult) -1;
    }
    for (uint8_t i = 0; i < imageCount; i++) {
      VkImageViewCreateInfo imageViewInfo = {};
      imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      imageViewInfo.pNext = NULL;
      imageViewInfo.flags = 0;
      imageViewInfo.image = images[i];
      imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      imageViewInfo.format = format;
      imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
      imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
      imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
      imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
      imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      imageViewInfo.subresourceRange.baseMipLevel = 0;
      imageViewInfo.subresourceRange.levelCount = 1;
      imageViewInfo.subresourceRange.baseArrayLayer = 0;
      imageViewInfo.subresourceRange.layerCount = 1;
      if (vkCreateImageView(device, &imageViewInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
        logger->error("Swapchain image view creation failed.");
      }
    }
    return VK_SUCCESS;
  }
  bool memoryTypeFromProperties(const VkPhysicalDevice& physicalDevice, uint32_t typeBits, VkFlags reqMask, uint32_t *typeIndex) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
      if ((typeBits & 1) == 1) {
        if ((memProps.memoryTypes[i].propertyFlags & reqMask) == reqMask) {
          *typeIndex = i;
          return true;
        }
      }
      typeBits >>= 1;
    }
    return false;
  }
  VkResult createDepthBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device, uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkImage& depthImage, VkDeviceMemory& deviceMemory, VkImageView& depthImageView) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_D16_UNORM;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = numSamples;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = nullptr;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = 0;
    if (vkCreateImage(device, &imageInfo, nullptr, &depthImage) != VK_SUCCESS) {
      logger->error("Depth buffer creation failed");
      return (VkResult)-1;
    }
    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device, depthImage, &memReqs);
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memReqs.size;
    allocateInfo.memoryTypeIndex = memoryTypeFromProperties(physicalDevice, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocateInfo.memoryTypeIndex);
    if (vkAllocateMemory(device, &allocateInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
      logger->error("Depth buffer creation failed");
      return (VkResult)-1;
    }
    if (vkBindImageMemory(device, depthImage, deviceMemory, 0) != VK_SUCCESS) {
      if (vkAllocateMemory(device, &allocateInfo, nullptr, &deviceMemory)) {
        logger->error("Depth buffer creation failed");
        return (VkResult)-1;
      }
    }
    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.pNext = nullptr;
    imageViewInfo.image = depthImage;
    imageViewInfo.format = VK_FORMAT_D16_UNORM;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.flags = 0;
    return vkCreateImageView(device, &imageViewInfo, nullptr, &depthImageView);
  }
  VkResult createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkBufferUsageFlags usage, void* data, size_t size, VkBuffer& buffer, VkDeviceMemory& deviceMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.usage = usage;
    bufferInfo.size = size;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
      logger->error("Buffer creation failed");
    }
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device, buffer, &memReqs);
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.allocationSize = memReqs.size;
    memoryTypeFromProperties(physicalDevice, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocateInfo.memoryTypeIndex);
    if (vkAllocateMemory(device, &allocateInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
      logger->error("Buffer creation failed");
    }
    void* mappedMem;
    if (vkMapMemory(device, deviceMemory, 0, memReqs.size, 0, (void **)&mappedMem) != VK_SUCCESS) {
      logger->error("Buffer creation failed");
    }
    memcpy(mappedMem, data, size);
    vkUnmapMemory(device, deviceMemory);
    return vkBindBufferMemory(device, buffer, deviceMemory, 0);
  }
  VkResult createPipelineLayout(const VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout) {
    VkDescriptorSetLayoutBinding layoutBindingInfo = {};
    layoutBindingInfo.binding = 0;
    layoutBindingInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindingInfo.descriptorCount = 1;
    layoutBindingInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.pNext = nullptr;
    descriptorLayoutInfo.bindingCount = 1;
    descriptorLayoutInfo.pBindings = &layoutBindingInfo;
    if (vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
      logger->error("Descriptor set layout creation failed.");
    }
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = 1;
    pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    return vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
  }
  VkResult createDescriptorPool(const VkDevice& device, const VkDescriptorPoolSize& descriptorPoolSize, VkDescriptorPool& descriptorPool) {
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &descriptorPoolSize;
    return vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool);
  }
  VkResult createDescriptorSet(const VkDevice& device, const VkDescriptorPool& descriptorPool, const VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorSet& descriptorSet) {
    VkDescriptorSetAllocateInfo allocateInfo[1];
    allocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo[0].pNext = nullptr;
    allocateInfo[0].descriptorPool = descriptorPool;
    allocateInfo[0].descriptorSetCount = 1;
    allocateInfo[0].pSetLayouts = &descriptorSetLayout;
    return vkAllocateDescriptorSets(device, allocateInfo, &descriptorSet);
  }
  void updateDescriptorSet(const VkDevice& device, const VkDescriptorSet& descriptorSet, const VkDescriptorBufferInfo& bufferInfo) {
    VkWriteDescriptorSet writes[1];
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptorSet;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &bufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;
    vkUpdateDescriptorSets(device, 1, writes, 0, nullptr);
  }
  void setImageLayout(const VkCommandBuffer& commandBuffer, const VkQueue& queue, const VkImage& image, VkImageAspectFlags aspectMask, const VkImageLayout& oldImageLayout, const VkImageLayout& newImageLayout, VkPipelineStageFlags srcStages, VkPipelineStageFlags destStages) {
    assert(commandBuffer != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = nullptr;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = 0;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    switch (oldImageLayout) {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      break;
    default:
      break;
    }
    switch (newImageLayout) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;
    default:
      break;
    }
    vkCmdPipelineBarrier(commandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
  }
  VkSubpassDescription createSubpassDescription(bool bAddDepthAttachment) {
    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = bAddDepthAttachment ? &depth_reference : nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;
    return subpass;
  }
  VkResult createRenderPass(const VkDevice& device, const std::vector<VkSubpassDescription>& subpasses, const std::vector<VkAttachmentDescription>& attachments, VkRenderPass& renderPass) {
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = nullptr;
    rp_info.attachmentCount = attachments.size();
    rp_info.pAttachments = attachments.data();
    rp_info.subpassCount = subpasses.size();
    rp_info.pSubpasses = subpasses.data();
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = nullptr;
    return vkCreateRenderPass(device, &rp_info, nullptr, &renderPass);
  }
  VkResult createShaderModule(const VkDevice& device, const std::vector<unsigned int>& spirV, VkShaderModule& shaderModule) {
    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = spirV.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = spirV.data();
    return vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);
  }
  VkPipelineShaderStageCreateInfo createShaderStage(VkShaderStageFlagBits stage) {
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.pNext = nullptr;
    shaderStageCreateInfo.pSpecializationInfo = nullptr;
    shaderStageCreateInfo.flags = 0;
    shaderStageCreateInfo.stage = stage;
    shaderStageCreateInfo.pName = "main";
    return shaderStageCreateInfo;
  }
  VkResult createFramebuffers(const VkDevice& device, const VkRenderPass& renderPass, const std::vector<VkImageView>& colorAttachments, const VkImageView& depthAttachment, uint32_t width, uint32_t height, std::vector<VkFramebuffer>& framebuffers) {
    VkImageView attachments[2];
    attachments[1] = depthAttachment;
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.pNext = nullptr;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = colorAttachments.size();
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;
    VkResult res;
    for (int i = 0; i < colorAttachments.size(); i++) {
      attachments[0] = colorAttachments[i];
      if ((res = vkCreateFramebuffer(device, &framebufferInfo, NULL, &framebuffers[i])) != VK_SUCCESS) {
        logger->error("Framebuffer creation failed");
      }
    }
    return res;
  }
  VkResult createGraphicsPipeline(
      const VkDevice& device, const VkPipelineLayout& pipelineLayout, 
      const VkPipelineVertexInputStateCreateInfo& vertexInputState,
      const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState,
      const VkPipelineRasterizationStateCreateInfo& rasterizationState,
      const VkPipelineColorBlendStateCreateInfo& colorBlendState,
      const VkPipelineMultisampleStateCreateInfo& multisampleState,
      const VkPipelineDynamicStateCreateInfo& dynamicState,
      const VkPipelineViewportStateCreateInfo& viewportState,
      const VkPipelineDepthStencilStateCreateInfo& depthStencilState,
      std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
      const VkRenderPass& renderPass, VkPipeline& pipeline) {
    VkGraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    return vkCreateGraphicsPipelines(device, NULL, 1, &pipelineCreateInfo, nullptr, &pipeline);
  }
  VkClearValue* clearValues(const VkClearColorValue& clearColor = {0, 0, 0, 0}, const VkClearDepthStencilValue& clearDepthStencil = { 1.f, 0 }) {
    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = clearColor.float32[0];
    clearValues[0].color.float32[1] = clearColor.float32[1];
    clearValues[0].color.float32[2] = clearColor.float32[2];
    clearValues[0].color.float32[3] = clearColor.float32[3];
    clearValues[1].depthStencil.depth = clearDepthStencil.depth;
    clearValues[1].depthStencil.stencil = clearDepthStencil.stencil;
    return clearValues;
  }
  void beginRenderPass(const VkCommandBuffer& commandBuffer, const VkRenderPass& renderPass, const VkFramebuffer& framebuffer, const VkExtent2D& renderArea, const VkClearValue* clearValues) {
    VkRenderPassBeginInfo renderPassBegin;
    renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBegin.pNext = nullptr;
    renderPassBegin.renderPass = renderPass;
    renderPassBegin.framebuffer = framebuffer;
    renderPassBegin.renderArea.offset.x = 0;
    renderPassBegin.renderArea.offset.y = 0;
    renderPassBegin.renderArea.extent.width = renderArea.width;
    renderPassBegin.renderArea.extent.height = renderArea.height;
    renderPassBegin.clearValueCount = 2;
    renderPassBegin.pClearValues = clearValues;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
  }
  VkResult submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer& commandBuffer, const VkSemaphore& waitSemaphore, VkPipelineStageFlags stage, VkFence& fence) {
    VkSubmitInfo submitInfo = {};
    submitInfo.pNext = nullptr;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = &stage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    return vkQueueSubmit(queue, 1, &submitInfo, fence);
  }

  VkResult presentSwapchain(const VkQueue& queue, const VkSwapchainKHR& swapchain, uint32_t imageIndex) {
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &swapchain;
    present.pImageIndices = &imageIndex;
    present.pWaitSemaphores = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults = nullptr;
    return vkQueuePresentKHR(queue, &present);
  }
}