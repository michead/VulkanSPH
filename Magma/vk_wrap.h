#pragma once

#include <iostream>
#include <vector>
#include "vk_context.h"
#include "logger.h"

#define VK_VER_MAJOR(X)   ((((uint32_t)(X)) >> 22) & 0x3FF)
#define VK_VER_MINOR(X)   ((((uint32_t)(X)) >> 12) & 0x3FF)
#define VK_VER_PATCH(X)   ((( uint32_t)(X))        & 0xFFF)
#define VK_CHECK(result)  if (result != VK_SUCCESS) { \
  logger->error("Vulkan API called failed with error %d", result); abort(); \
}

namespace VkWrap {
  inline void getAvailableWSIExtensions(VkContext* context) {
    context->extensions.clear();
    context->extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    context->extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
  }
  inline void createInstance(VkContext* context, const char* appName, uint32_t appVersion) {
    // Use validation layers if this is a debug build, and use WSI extensions regardless
    getAvailableWSIExtensions(context);
    std::vector<const char*> layers;
#if defined(_DEBUG)
    layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = appVersion;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(context->extensions.size());
    instanceInfo.ppEnabledExtensionNames = context->extensions.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
    
    VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &context->instance));
  }
  inline void getDevices(VkContext* context) {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, nullptr);
    context->physicalDevices.resize(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &deviceCount, context->physicalDevices.data()));
  }
  inline void getQueueFamilyProperties(VkContext* context) {
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(context->physicalDevice, &familyCount, nullptr);
    context->queueFamilyProps.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(context->physicalDevice, &familyCount, context->queueFamilyProps.data());
  }
  inline void getQueueFamilyIndeices(VkContext* context) {
    getQueueFamilyProperties(context);
    context->graphicsQueueFamilyIndex = UINT32_MAX;
    context->presentQueueFamilyIndex = UINT32_MAX;
    uint32_t queueFamilyCount = context->queueFamilyProps.size();
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
      vkGetPhysicalDeviceSurfaceSupportKHR(context->physicalDevice, i, context->surface, &pSupportsPresent[i]);
    }
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
      if ((context->queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
        if (context->graphicsQueueFamilyIndex == UINT32_MAX) {
          context->graphicsQueueFamilyIndex = i;
        }
        if (pSupportsPresent[i] == VK_TRUE) {
          context->graphicsQueueFamilyIndex = i;
          context->presentQueueFamilyIndex = i;
          break;
        }
      }
    }
    if (context->presentQueueFamilyIndex == UINT32_MAX) {
      for (size_t i = 0; i < queueFamilyCount; i++) {
        if (pSupportsPresent[i] == VK_TRUE) {
          context->presentQueueFamilyIndex = i;
          break;
        }
      }
    }
    if (context->graphicsQueueFamilyIndex < 0 ||
        context->presentQueueFamilyIndex  < 0) {
      logger->error("No suitable device for performing graphics operations has been found.");
      assert(false);
    }
  }
  inline void createSemaphore(VkContext* context, VkSemaphore& semaphore) {
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;
    VK_CHECK(vkCreateSemaphore(context->device, &imageAcquiredSemaphoreCreateInfo, nullptr, &semaphore));
  }
  inline void createFence(VkContext* context, VkFence& fence) {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    VK_CHECK(vkCreateFence(context->device, &fenceInfo, nullptr, &fence));
  }
  inline void createDevice(VkContext* context) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(context->physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<const char *> extensions;
    std::vector<VkExtensionProperties> extensionProps(extensionCount);
    vkEnumerateDeviceExtensionProperties(context->physicalDevice, nullptr, &extensionCount, extensionProps.data());
    for (uint32_t i = 0; i < extensionCount; i++) {
      extensions.push_back(extensionProps[i].extensionName);
    }
    
    VkDeviceQueueCreateInfo queueInfo = {};
    float queue_priorities[1] = { 0.0 };
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queue_priorities;
    queueInfo.queueFamilyIndex = context->graphicsQueueFamilyIndex;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueInfo;
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    deviceCreateInfo.pEnabledFeatures = nullptr;

    VK_CHECK(vkCreateDevice(context->physicalDevice, &deviceCreateInfo, nullptr, &context->device));

    vkGetDeviceQueue(context->device, context->graphicsQueueFamilyIndex, 0, &context->graphicsQueue);
    if (context->graphicsQueueFamilyIndex != context->presentQueueFamilyIndex) {
      context->presentQueue = context->graphicsQueue;
    } else {
      vkGetDeviceQueue(context->device, context->presentQueueFamilyIndex, 0, &context->presentQueue);
    }
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
  inline void createCommandPool(VkContext* context) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = context->graphicsQueueFamilyIndex;
    info.flags = 0;
    VK_CHECK(vkCreateCommandPool(context->device, &info, nullptr, &context->commandPool));
  }
  inline void createCommandBuffer(VkContext* context, size_t bufferCount, VkCommandBuffer* commandBuffers) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = context->commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = bufferCount;
    VK_CHECK(vkAllocateCommandBuffers(context->device, &info, commandBuffers));
  }
  inline void createSurface(VkContext* context, HWND hwnd) {
#if defined(SDL_VIDEO_DRIVER_WINDOWS) && defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(NULL);
    surfaceInfo.hwnd = hwnd;
    VK_CHECK(vkCreateWin32SurfaceKHR(context->instance, &surfaceInfo, NULL, &context->surface));
#endif
  }
  inline void createSwapchain(VkContext* context) {
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context->physicalDevice, context->surface, &surfaceFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(context->physicalDevice, context->surface, &surfaceFormatCount, surfaceFormats.data());
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physicalDevice, context->surface, &surfaceCapabilities);
    VkBool32 surfaceSupported;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context->physicalDevice,
                                                  context->graphicsQueueFamilyIndex,
                                                  context->surface, &surfaceSupported));
    if (!surfaceSupported || surfaceFormatCount < 1) {
      logger->error("Surface is not supported. Cannot create swapchain.");
      assert(false);
    }
    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
      context->format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
      context->format = surfaceFormats[0].format;
    }

    uint32_t presentModeCount;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physicalDevice, context->surface, &presentModeCount, nullptr));
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physicalDevice, context->surface, &presentModeCount, presentModes.data()));

    VkExtent2D swapchainExtent;
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
      swapchainExtent.width  = 1920;
      swapchainExtent.height = 1080;
      if (swapchainExtent.width < surfaceCapabilities.minImageExtent.width) {
        swapchainExtent.width = surfaceCapabilities.minImageExtent.width;
      } else if (swapchainExtent.width > surfaceCapabilities.maxImageExtent.width) {
        swapchainExtent.width = surfaceCapabilities.maxImageExtent.width;
      }

      if (swapchainExtent.height < surfaceCapabilities.minImageExtent.height) {
        swapchainExtent.height = surfaceCapabilities.minImageExtent.height;
      } else if (swapchainExtent.height > surfaceCapabilities.maxImageExtent.height) {
        swapchainExtent.height = surfaceCapabilities.maxImageExtent.height;
      }
    } else {
      swapchainExtent = surfaceCapabilities.currentExtent;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    uint32_t desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount;
    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
      preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
      preTransform = surfaceCapabilities.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
      if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
        compositeAlpha = compositeAlphaFlags[i];
        break;
      }
    }

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.surface = context->surface;
    swapchainInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapchainInfo.imageFormat = context->format;
    swapchainInfo.imageExtent.width = swapchainExtent.width;
    swapchainInfo.imageExtent.height = swapchainExtent.height;
    swapchainInfo.preTransform = preTransform;
    swapchainInfo.compositeAlpha = compositeAlpha;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.presentMode = swapchainPresentMode;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.clipped = true;
    swapchainInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    uint32_t queueFamilyIndices[2] = {
      context->graphicsQueueFamilyIndex,
      context->presentQueueFamilyIndex
    };
    if (context->graphicsQueueFamilyIndex != context->presentQueueFamilyIndex) {
      swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchainInfo.queueFamilyIndexCount = 2;
      swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    VK_CHECK(vkCreateSwapchainKHR(context->device, &swapchainInfo, nullptr, &context->swapchain));
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, context->swapchain, &context->swapchainImageCount, nullptr));

    std::vector<VkImage> swapchainImages(context->swapchainImageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, context->swapchain, &context->swapchainImageCount, swapchainImages.data()));

    context->swapchainImages.resize(context->swapchainImageCount);
    context->swapchainImageViews.resize(context->swapchainImageCount);

    for (uint32_t i = 0; i < context->swapchainImageCount; i++) {
      VkImageViewCreateInfo colorImageView = {};
      colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      colorImageView.pNext = NULL;
      colorImageView.format = context->format;
      colorImageView.components.r = VK_COMPONENT_SWIZZLE_R;
      colorImageView.components.g = VK_COMPONENT_SWIZZLE_G;
      colorImageView.components.b = VK_COMPONENT_SWIZZLE_B;
      colorImageView.components.a = VK_COMPONENT_SWIZZLE_A;
      colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      colorImageView.subresourceRange.baseMipLevel = 0;
      colorImageView.subresourceRange.levelCount = 1;
      colorImageView.subresourceRange.baseArrayLayer = 0;
      colorImageView.subresourceRange.layerCount = 1;
      colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
      colorImageView.flags = 0;

      context->swapchainImages[i] = swapchainImages[i];
      colorImageView.image = swapchainImages[i];

      VK_CHECK(vkCreateImageView(context->device, &colorImageView, nullptr, &context->swapchainImageViews[i]));
    }
    context->currentSwapchainImageIndex = 0;
  }
  inline void getMemoryTypeFromProperties(VkContext* context, uint32_t typeBits, VkFlags reqMask, uint32_t* memoryTypeIndex) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &memProps);
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
      if ((typeBits & 1) == 1) {
        if ((memProps.memoryTypes[i].propertyFlags & reqMask) == reqMask) {
          *memoryTypeIndex = i;
          return;
        }
      }
      typeBits >>= 1;
    }
    assert(false);
  }
  inline void createDepthBuffer(VkContext* context, VkSampleCountFlagBits numSamples, VkImage& depthImage, VkDeviceMemory& deviceMemory, VkImageView& depthImageView) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_D16_UNORM;
    imageInfo.extent.width = context->renderArea.width;
    imageInfo.extent.height = context->renderArea.height;
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
    VK_CHECK(vkCreateImage(context->device, &imageInfo, nullptr, &depthImage));

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(context->device, depthImage, &memReqs);
    
    uint32_t memoryTypeIndex;
    getMemoryTypeFromProperties(context, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memoryTypeIndex);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memReqs.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    VK_CHECK(vkAllocateMemory(context->device, &allocateInfo, nullptr, &deviceMemory));
    VK_CHECK(vkBindImageMemory(context->device, depthImage, deviceMemory, 0));
    
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
    VK_CHECK(vkCreateImageView(context->device, &imageViewInfo, nullptr, &depthImageView));
  }
  inline void createBuffer(VkContext* context, VkBufferUsageFlags usage, void* data, size_t size, VkBuffer& buffer, VkDeviceMemory& deviceMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.usage = usage;
    bufferInfo.size = size;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;
    VK_CHECK(vkCreateBuffer(context->device, &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(context->device, buffer, &memReqs);
    
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.allocationSize = memReqs.size;
    
    uint32_t memoryTypeIndex;
    getMemoryTypeFromProperties(context, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memoryTypeIndex);
    
    VK_CHECK(vkAllocateMemory(context->device, &allocateInfo, nullptr, &deviceMemory));
    
    void* mappedMem;
    VK_CHECK(vkMapMemory(context->device, deviceMemory, 0, memReqs.size, 0, (void**)&mappedMem));

    memcpy(mappedMem, data, size);
    vkUnmapMemory(context->device, deviceMemory);
    VK_CHECK(vkBindBufferMemory(context->device, buffer, deviceMemory, 0));
  }
  inline void createPipelineLayout(VkContext* context, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout) {
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
    VK_CHECK(vkCreateDescriptorSetLayout(context->device, &descriptorLayoutInfo, nullptr, &descriptorSetLayout));
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount = 1;
    pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    VK_CHECK(vkCreatePipelineLayout(context->device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout));
  }
  inline void createDescriptorPool(VkContext* context, const VkDescriptorPoolSize& descriptorPoolSize) {
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &descriptorPoolSize;
    VK_CHECK(vkCreateDescriptorPool(context->device, &descriptorPoolInfo, nullptr, &context->descriptorPool));
  }
  inline void createDescriptorSet(VkContext* context, const VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorSet& descriptorSet) {
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorPool = context->descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &descriptorSetLayout;
    VK_CHECK(vkAllocateDescriptorSets(context->device, &allocateInfo, &descriptorSet));
  }
  inline void updateDescriptorSet(VkContext* context, const VkDescriptorSet& descriptorSet, const VkDescriptorBufferInfo& bufferInfo) {
    VkWriteDescriptorSet writes = {};
    writes = {};
    writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes.pNext = nullptr;
    writes.dstSet = descriptorSet;
    writes.descriptorCount = 1;
    writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes.pBufferInfo = &bufferInfo;
    writes.dstArrayElement = 0;
    writes.dstBinding = 0;
    vkUpdateDescriptorSets(context->device, 1, &writes, 0, nullptr);
  }
  inline void setImageLayout(const VkCommandBuffer& commandBuffer,
                      const VkImage& image,
                      VkImageAspectFlags aspectMask,
                      const VkImageLayout& oldImageLayout,
                      const VkImageLayout& newImageLayout,
                      VkPipelineStageFlags srcStages,
                      VkPipelineStageFlags destStages) {
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
  inline void createSubpassDescription(bool bAddDepthAttachment, VkSubpassDescription& description) {
    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    description = {};
    description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    description.flags = 0;
    description.inputAttachmentCount = 0;
    description.pInputAttachments = nullptr;
    description.colorAttachmentCount = 1;
    description.pColorAttachments = &color_reference;
    description.pResolveAttachments = nullptr;
    description.pDepthStencilAttachment = bAddDepthAttachment ? &depth_reference : nullptr;
    description.preserveAttachmentCount = 0;
    description.pPreserveAttachments = nullptr;
  }
  inline void createRenderPass(const VkContext* context,
                               const std::vector<VkSubpassDescription>& subpasses,
                               const std::vector<VkAttachmentDescription>& attachments,
                               VkRenderPass& renderPass) {
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = nullptr;
    rp_info.attachmentCount = attachments.size();
    rp_info.pAttachments = attachments.data();
    rp_info.subpassCount = subpasses.size();
    rp_info.pSubpasses = subpasses.data();
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = nullptr;
    VK_CHECK(vkCreateRenderPass(context->device, &rp_info, nullptr, &renderPass));
  }
  inline void createShaderModule(const VkContext* context, const std::vector<unsigned int>& spirV, VkShaderModule& shaderModule) {
    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = spirV.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = spirV.data();
    VK_CHECK(vkCreateShaderModule(context->device, &moduleCreateInfo, nullptr, &shaderModule));
  }
  inline void createShaderStage(VkShaderStageFlagBits stage, VkPipelineShaderStageCreateInfo& shaderStage) {
    shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.pNext = nullptr;
    shaderStage.pSpecializationInfo = nullptr;
    shaderStage.flags = 0;
    shaderStage.stage = stage;
    shaderStage.pName = "main";
  }
  inline void createFramebuffers(const VkContext* context,
                              const VkRenderPass& renderPass,
                              const std::vector<VkImageView>& colorAttachments,
                              const VkImageView& depthAttachment,
                              uint32_t width,
                              uint32_t height,
                              std::vector<VkFramebuffer>& framebuffers) {
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
    for (size_t i = 0; i < colorAttachments.size(); i++) {
      attachments[0] = colorAttachments[i];
      VK_CHECK(vkCreateFramebuffer(context->device, &framebufferInfo, NULL, &framebuffers[i]));
    }
  }
  inline void createGraphicsPipeline(
      const VkContext* context,
      const VkPipelineLayout& pipelineLayout, 
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
    VK_CHECK(vkCreateGraphicsPipelines(context->device, NULL, 1, &pipelineCreateInfo, nullptr, &pipeline));
  }
  inline void clearValues(std::array<VkClearValue, 2>& clearValues,
                          const VkClearColorValue& clearColor = {0, 0, 0, 0},
                          const VkClearDepthStencilValue& clearDepthStencil = { 1.f, 0 }) {
    clearValues[0].color.float32[0] = clearColor.float32[0];
    clearValues[0].color.float32[1] = clearColor.float32[1];
    clearValues[0].color.float32[2] = clearColor.float32[2];
    clearValues[0].color.float32[3] = clearColor.float32[3];
    clearValues[1].depthStencil.depth = clearDepthStencil.depth;
    clearValues[1].depthStencil.stencil = clearDepthStencil.stencil;
  }
  inline void beginRenderPass(const VkCommandBuffer& commandBuffer,
                              const VkRenderPass& renderPass,
                              const VkFramebuffer& framebuffer,
                              const VkExtent2D& renderArea,
                              const VkClearValue* clearValues) {
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
  inline void submitCommandBuffer(VkContext* context,
                                  const VkCommandBuffer& commandBuffer,
                                  const VkSemaphore& waitSemaphore,
                                  VkPipelineStageFlags stage,
                                  VkFence& fence) {
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
    VK_CHECK(vkQueueSubmit(context->graphicsQueue, 1, &submitInfo, fence));
  }

  inline void presentSwapchain(VkContext* context) {
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &context->swapchain;
    present.pImageIndices = &context->currentSwapchainImageIndex;
    present.pWaitSemaphores = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults = nullptr;
    VK_CHECK(vkQueuePresentKHR(context->graphicsQueue, &present));
  }
  inline void compileShaders(LPCWSTR dir) {
    HANDLE hFind;
    WIN32_FIND_DATA data;
    hFind = FindFirstFile(dir, &data);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        char filename[128];
        sprintf_s(filename, "%ws", data.cFileName);
        logger->info("Compiling shader %s ...", filename);
        char command[64];
        sprintf_s(command, "glslangValidator %s", filename);
        system(command);
      } while (FindNextFile(hFind, &data));
      FindClose(hFind);
    }
    logger->info("Shader compilation complete.");
  }
  inline void prepareFrame(VkContext* context) {
    VK_CHECK(vkAcquireNextImageKHR(context->device,
                                   context->swapchain, UINT64_MAX,
                                   context->imageAcquiredSemaphore, VK_NULL_HANDLE,
                                  &context->currentSwapchainImageIndex));
  }
}