#pragma once

#include <vulkan\vulkan.hpp>
#include <iostream>
#include <vector>
#include "gfx_structs.h"
#include "logger.h"

#define VK_VER_MAJOR(X)   ((((uint32_t)(X)) >> 22) & 0x3FF)
#define VK_VER_MINOR(X)   ((((uint32_t)(X)) >> 12) & 0x3FF)
#define VK_VER_PATCH(X)   ((( uint32_t)(X))        & 0xFFF)
#define VK_CHECK(result)                   assert((result) == VK_SUCCESS)
#define VK_CHECK_PRESENT_TOLERANT(result)  assert((result) == VK_SUCCESS || (result) == VK_ERROR_OUT_OF_DATE_KHR || (result) == VK_SUBOPTIMAL_KHR)
#define VK_ASSERT(handler)                 assert((handler) != VK_NULL_HANDLE)
#define VK_REGISTER(type, count, instances) { }

#define VK_DESCRIPTOR_POOL_MAX_SETS_DEFAULT 30
#define VK_DESCRIPTOR_POOL_SIZE_DEFAULT     1

namespace GfxWrap {
  inline void queryAvailableWSIExtensions(std::vector<const char*>& extensions) {
    extensions.clear();
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
  }
  inline void createInstance(const char* appName, uint32_t appVersion, VkInstance& instance) {
    std::vector<const char*> extensions;
    queryAvailableWSIExtensions(extensions);
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
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
    
    VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &instance));
  }
  inline void queryDevices(VkInstance instance, std::vector<VkPhysicalDevice>& physicalDevices) {
    uint32_t deviceCount;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
    physicalDevices.resize(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data()));
  }
  inline void queryQueueFamilyProps(VkPhysicalDevice physicalDevice, std::vector<VkQueueFamilyProperties>& queueFamilyProps) {
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
    queueFamilyProps.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueFamilyProps.data());
  }
  inline void queryQueueFamilyIndices(VkPhysicalDevice physicalDevice,
                                      VkSurfaceKHR surface,
                                      uint32_t* graphicsQueueFamilyIndex,
                                      uint32_t* presentQueueFamilyIndex) {
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
    queryQueueFamilyProps(physicalDevice, queueFamilyProps);
    
    *graphicsQueueFamilyIndex = UINT32_MAX;
    *presentQueueFamilyIndex = UINT32_MAX;
    uint32_t queueFamilyCount = queueFamilyProps.size();
    
    std::vector<VkBool32> supportsPresent(queueFamilyCount);
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]);
    }
    
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
      if ((queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
        if (*graphicsQueueFamilyIndex == UINT32_MAX) {
          *graphicsQueueFamilyIndex = i;
        }
        if (supportsPresent[i] == VK_TRUE) {
          *graphicsQueueFamilyIndex = i;
          *presentQueueFamilyIndex = i;
          break;
        }
      }
    }
    
    if (*presentQueueFamilyIndex == UINT32_MAX) {
      for (size_t i = 0; i < queueFamilyCount; i++) {
        if (supportsPresent[i] == VK_TRUE) {
          *presentQueueFamilyIndex = i;
          break;
        }
      }
    }
    
    if (graphicsQueueFamilyIndex < 0 ||
        presentQueueFamilyIndex  < 0) {
      logger->error("No suitable device for performing graphics operations has been found.");
      exit(EXIT_FAILURE);
    }
  }
  inline void queryDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* deviceFeatures) {
    vkGetPhysicalDeviceFeatures(physicalDevice, deviceFeatures);
  }
  inline void createSemaphore(VkDevice device, VkSemaphore& semaphore) {
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;
    VK_CHECK(vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, nullptr, &semaphore));
  }
  inline void createFence(VkDevice device, VkFence& fence) {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &fence));
  }
  inline void createDevice(VkPhysicalDevice physicalDevice,
                           uint32_t graphicsQueueFamilyIndex,
                           uint32_t presentQueueFamilyIndex,
                           VkDevice& device,
                           VkQueue& graphicsQueue,
                           VkQueue& presentQueue) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<const char *> extensions;
    std::vector<VkExtensionProperties> extensionProps(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProps.data());
    for (uint32_t i = 0; i < extensionCount; i++) {
      extensions.push_back(extensionProps[i].extensionName);
    }
    
    VkDeviceQueueCreateInfo queueInfo = {};
    float queue_priorities[1] = { 0.0 };
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queue_priorities;
    queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueInfo;
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    deviceCreateInfo.pEnabledFeatures = nullptr;

    VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
      presentQueue = graphicsQueue;
    } else {
      vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
    }
  }
  inline void printDeviceStats(const VkPhysicalDevice& physicalDevice) {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    logger->info("Driver version {0}", physicalDeviceProperties.driverVersion);
    logger->info("Device name {0}", physicalDeviceProperties.deviceName);
    logger->info("Device type {0}", physicalDeviceProperties.deviceType);
    logger->info("API version {0:d}.{0:d}.{0:d}", VK_VER_MAJOR(physicalDeviceProperties.apiVersion),
                                                  VK_VER_MINOR(physicalDeviceProperties.apiVersion),
                                                  VK_VER_PATCH(physicalDeviceProperties.apiVersion));
  }
  inline void createCommandPool(VkDevice device, uint32_t graphicsQueueFamilyIndex, VkCommandPool& commandPool) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = graphicsQueueFamilyIndex;
    info.flags = 0;
    VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &commandPool));
  }
  inline void createCommandBuffers(VkDevice device,
                                   VkCommandPool commandPool,
                                   uint32_t bufferCount,
                                   VkCommandBuffer* commandBuffers) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = bufferCount;
    VK_CHECK(vkAllocateCommandBuffers(device, &info, commandBuffers));
  }
  inline void createSurface(VkInstance instance, 
                            VkPhysicalDevice physicalDevice, 
                            HWND hwnd,
                            uint32_t* graphicsQueueFamilyIndex,
                            uint32_t* presentQueueFamilyIndex,
                            VkSurfaceKHR& surface,
                            VkSurfaceCapabilitiesKHR& surfaceCapabilities,
                            VkFormat& format) {
#if defined(SDL_VIDEO_DRIVER_WINDOWS) && defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(NULL);
    surfaceInfo.hwnd = hwnd;
    VK_CHECK(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface));
#endif
    
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
    uint32_t surfaceFormatCount;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));
    
    queryQueueFamilyIndices(physicalDevice, surface, graphicsQueueFamilyIndex, presentQueueFamilyIndex);

    VkBool32 surfaceSupported;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, *graphicsQueueFamilyIndex, surface, &surfaceSupported));
    if (!surfaceSupported || surfaceFormatCount < 1) {
      logger->error("Surface is not supported. Cannot create swapchain.");
      exit(EXIT_FAILURE);
    }
    
    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
      format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
      format = surfaceFormats[0].format;
    }

    uint32_t presentModeCount;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr));
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));
  }
  inline void beginCommandBuffer(VkCommandBuffer commandBuffer,
                                 VkCommandBufferUsageFlags flags = 0) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext            = nullptr;
    beginInfo.pInheritanceInfo = nullptr;
    beginInfo.flags            = flags;
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
  }
  inline void createSwapchain(VkDevice device,
                              VkSurfaceKHR surface,
                              VkSurfaceCapabilitiesKHR surfaceCapabilities,
                              VkFormat format,
                              uint32_t graphicsQueueFamilyIndex,
                              uint32_t presentQueueFamilyIndex,
                              VkSwapchainKHR& swapchain,
                              VkExtent2D& swapchainExtent,
                              std::vector<VkImage>& swapchainImages,
                              std::vector<VkImageView>& swapchainImageViews) {
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
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
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapchainInfo.imageFormat = format;
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
      graphicsQueueFamilyIndex,
      presentQueueFamilyIndex
    };
    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
      swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchainInfo.queueFamilyIndexCount = 2;
      swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    uint32_t swapchainImageCount;
    VK_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain));
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr));

    swapchainImages.resize(swapchainImageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data()));

    swapchainImages.resize(swapchainImageCount);
    swapchainImageViews.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
      VkImageViewCreateInfo colorImageView = {};
      colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      colorImageView.pNext = nullptr;
      colorImageView.format = format;
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
      colorImageView.image = swapchainImages[i];

      VK_CHECK(vkCreateImageView(device, &colorImageView, nullptr, &swapchainImageViews[i]));
    }
  }
  inline void getMemTypeIndexFromMemProps(VkPhysicalDevice physicalDevice, uint32_t typeBits,
                                          VkFlags reqMask, uint32_t* memoryTypeIndex) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
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
  inline void queryImageFormatProps(VkPhysicalDevice physicalDevice,
                                    VkFormat format,
                                    VkImageType imageType,
                                    VkImageTiling imageTiling,
                                    VkImageUsageFlags usage,
                                    VkImageCreateFlags createFlags,
                                    VkImageFormatProperties& imageFormatProps) {
    VK_CHECK(vkGetPhysicalDeviceImageFormatProperties(
      physicalDevice,
      format,
      imageType,
      imageTiling,
      usage,
      createFlags,
      &imageFormatProps));
  }
  inline void createDepthBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D renderArea,
                                VkSampleCountFlagBits numSamples, VkImage& depthImage, 
                                VkDeviceMemory& deviceMemory, VkImageView& depthImageView) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_D16_UNORM;
    imageInfo.extent.width = renderArea.width;
    imageInfo.extent.height = renderArea.height;
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
    VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &depthImage));

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device, depthImage, &memReqs);
    
    uint32_t memoryTypeIndex;
    getMemTypeIndexFromMemProps(physicalDevice, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memoryTypeIndex);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memReqs.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    VK_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &deviceMemory));
    VK_CHECK(vkBindImageMemory(device, depthImage, deviceMemory, 0));
    
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
    VK_CHECK(vkCreateImageView(device, &imageViewInfo, nullptr, &depthImageView));
  }
  inline void allocateDeviceMemory(VkPhysicalDevice physicalDevice,
                                   VkDevice device,
                                   VkBuffer buffer,
                                   VkDeviceSize* allocSize,
                                   VkDeviceMemory& deviceMemory,
                                   VkMemoryPropertyFlags memProps) {
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device, buffer, &memReqs);

    uint32_t memoryTypeIndex;
    getMemTypeIndexFromMemProps(
      physicalDevice, memReqs.memoryTypeBits,
      memProps,
      &memoryTypeIndex);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;
    allocateInfo.allocationSize = memReqs.size;

    VK_CHECK(vkAllocateMemory(device, &allocateInfo, nullptr, &deviceMemory));
    *allocSize = memReqs.size;
  }
  inline void updateBuffer(VkDevice device,
                           size_t size,
                           void* data,
                           VkDeviceSize allocSize,
                           VkDeviceMemory deviceMemory,
                           void** mappedMemory) {
    VK_CHECK(vkMapMemory(device, deviceMemory, 0, allocSize, 0, mappedMemory));
    memcpy(*mappedMemory, data, size);
    vkUnmapMemory(device, deviceMemory);
  }
  inline void createBuffer(VkPhysicalDevice physicalDevice,
                           VkDevice device,
                           VkBufferUsageFlags usage,
                           void* data,
                           size_t size,
                           MVkBufferDesc* bufferDesc) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.usage = usage;
    bufferInfo.size = size;
    bufferInfo.flags = 0;
    VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &bufferDesc->buffer));

    GfxWrap::allocateDeviceMemory(
      physicalDevice,
      device,
      bufferDesc->buffer,
      &bufferDesc->allocSize,
      bufferDesc->deviceMemory,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    GfxWrap::updateBuffer(
      device,
      size,
      data,
      bufferDesc->allocSize,
      bufferDesc->deviceMemory,
      &bufferDesc->mappedMemory);
    
    VK_CHECK(vkBindBufferMemory(device, bufferDesc->buffer, bufferDesc->deviceMemory, 0));

    bufferDesc->bufferInfo.buffer = bufferDesc->buffer;
    bufferDesc->bufferInfo.offset = 0;
    bufferDesc->bufferInfo.range = size;
  }
  inline void submitCmdBuffer(VkDevice device,
                              VkQueue queue,
                              VkCommandBuffer cmdBuffer) {
                              VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(queue));
  }
  inline void registerCopyCmd(VkCommandBuffer cmdBuffer,
                              VkDeviceSize size,
                              VkQueue queue,
                              VkBuffer src,
                              VkBuffer dst) {
    beginCommandBuffer(cmdBuffer);
    VkBufferCopy copyRegion = { 0, 0, size };
    vkCmdCopyBuffer(cmdBuffer, src, dst, 1, &copyRegion);
    VK_CHECK(vkEndCommandBuffer(cmdBuffer));
  }
  inline void createStagingBuffer(VkPhysicalDevice physicalDevice,
                                  VkDevice device,
                                  VkBufferUsageFlags usage,
                                  void* data,
                                  size_t size,
                                  MVkBufferDesc* hostBuffer,
                                  MVkBufferDesc* deviceBuffer) {
    VkBufferCreateInfo hostBufferInfo = {};
    hostBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    hostBufferInfo.pNext = nullptr;
    hostBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    hostBufferInfo.size = size;
    hostBufferInfo.flags = 0;
    VK_CHECK(vkCreateBuffer(device, &hostBufferInfo, nullptr, &hostBuffer->buffer));

    GfxWrap::allocateDeviceMemory(
      physicalDevice,
      device,
      hostBuffer->buffer,
      &hostBuffer->allocSize,
      hostBuffer->deviceMemory,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    GfxWrap::updateBuffer(
      device,
      size,
      data,
      hostBuffer->allocSize,
      hostBuffer->deviceMemory,
      &hostBuffer->mappedMemory);

    VK_CHECK(vkBindBufferMemory(device, hostBuffer->buffer, hostBuffer->deviceMemory, 0));

    hostBuffer->bufferInfo.buffer = hostBuffer->buffer;
    hostBuffer->bufferInfo.offset = 0;
    hostBuffer->bufferInfo.range = size;

    VkBufferCreateInfo deviceBufferInfo = {};
    deviceBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    deviceBufferInfo.pNext = nullptr;
    deviceBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
    deviceBufferInfo.size = size;
    deviceBufferInfo.flags = 0;
    VK_CHECK(vkCreateBuffer(device, &deviceBufferInfo, nullptr, &deviceBuffer->buffer));

    GfxWrap::allocateDeviceMemory(
      physicalDevice,
      device,
      deviceBuffer->buffer,
      &deviceBuffer->allocSize,
      deviceBuffer->deviceMemory,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkBindBufferMemory(device, deviceBuffer->buffer, deviceBuffer->deviceMemory, 0));

    deviceBuffer->bufferInfo.buffer = deviceBuffer->buffer;
    deviceBuffer->bufferInfo.offset = 0;
    deviceBuffer->bufferInfo.range = size;
  }
  inline void createDescriptorSetLayout(VkDevice device,
                                        const std::vector<VkDescriptorSetLayoutBinding>& bindings,
                                        VkDescriptorSetLayout& descriptorSetLayout) {
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext        = nullptr;
    createInfo.bindingCount = bindings.size();
    createInfo.pBindings    = bindings.data();
    VK_CHECK(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayout));
  }
  inline void createPipelineLayout(const VkDevice& device,
                                   uint32_t descriptorSetLayoutCount,
                                   VkDescriptorSetLayout* descriptorSetLayouts,
                                   VkPipelineLayout& pipelineLayout) {
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext                      = nullptr;
    pPipelineLayoutCreateInfo.pushConstantRangeCount     = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges        = nullptr;
    pPipelineLayoutCreateInfo.setLayoutCount             = descriptorSetLayoutCount;
    pPipelineLayoutCreateInfo.pSetLayouts                = descriptorSetLayouts;
    VK_CHECK(vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout));
  }
  inline void createDescriptorPool(const VkDevice& device, VkDescriptorPoolSize descriptorPoolSize,
                                   VkDescriptorPool& descriptorPool) {
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext         = nullptr;
    descriptorPoolInfo.maxSets       = VK_DESCRIPTOR_POOL_MAX_SETS_DEFAULT;
    descriptorPoolInfo.poolSizeCount = VK_DESCRIPTOR_POOL_SIZE_DEFAULT;
    descriptorPoolInfo.pPoolSizes    = &descriptorPoolSize;
    VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool));
  }
  inline void createDescriptorSet(const VkDevice& device, VkDescriptorPool descriptorPool,
                                  VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptorSet) {
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext              = nullptr;
    allocateInfo.descriptorPool     = descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts        = &descriptorSetLayout;
    VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet));
  }
  inline void updateDescriptorSet(const VkDevice& device,
                                  VkDescriptorSet descriptorSet,
                                  uint32_t binding,
                                  const VkDescriptorBufferInfo& bufferInfo) {
    VkWriteDescriptorSet writes = {};
    writes.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes.pNext           = nullptr;
    writes.dstSet          = descriptorSet;
    writes.descriptorCount = 1;
    writes.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes.pBufferInfo     = &bufferInfo;
    writes.dstArrayElement = 0;
    writes.dstBinding      = binding;
    vkUpdateDescriptorSets(device, 1, &writes, 0, nullptr);
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
  inline void createRenderPass(const VkDevice device,
                               const std::vector<VkAttachmentDescription>& attachments,
                               const std::vector<VkSubpassDescription>& subpasses,
                               const std::vector<VkSubpassDependency>& dependencies,
                               VkRenderPass& renderPass) {
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pNext           = nullptr;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments    = attachments.data();
    renderPassInfo.subpassCount    = subpasses.size();
    renderPassInfo.pSubpasses      = subpasses.data();
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies   = dependencies.data();
    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
  }
  inline void createShaderModule(VkDevice device, const std::vector<uint32_t>& spirV, VkShaderModule& shaderModule) {
    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext    = nullptr;
    moduleCreateInfo.flags    = 0;
    moduleCreateInfo.codeSize = spirV.size() * sizeof(uint32_t);
    moduleCreateInfo.pCode    = spirV.data();
    VK_CHECK(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule));
  }
  inline void shaderStage(const VkShaderModule& module, VkShaderStageFlagBits stage, VkPipelineShaderStageCreateInfo& shaderStage) {
    shaderStage = {};
    shaderStage.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.pNext               = nullptr;
    shaderStage.pSpecializationInfo = nullptr;
    shaderStage.flags               = 0;
    shaderStage.stage               = stage;
    shaderStage.pName               = "main";
    shaderStage.module              = module;
  }
  inline void createFramebuffers(VkDevice device,
                                const VkRenderPass& renderPass,
                                const std::vector<VkImageView>& colorAttachments,
                                VkImageView depthAttachment,
                                uint32_t width,
                                uint32_t height,
                                std::vector<VkFramebuffer>& framebuffers) {
    std::vector<VkImageView> attachments(1);
    if (depthAttachment != VK_NULL_HANDLE) {
      attachments.push_back(depthAttachment);
    }
    
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.pNext           = nullptr;
    framebufferInfo.renderPass      = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = width;
    framebufferInfo.height          = height;
    framebufferInfo.layers          = 1;
    
    framebuffers.clear();
    framebuffers.resize(colorAttachments.size());

    for (size_t i = 0; i < colorAttachments.size(); i++) {
      attachments[0] = colorAttachments[i];
      VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, NULL, &framebuffers[i]));
    }
  }
  inline void createGraphicsPipeline(VkDevice device,
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
                                     VkRenderPass renderPass,
                                     VkPipelineCache pipelineCache,
                                     VkPipeline& pipeline) {
    VkGraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext               = nullptr;
    pipelineCreateInfo.layout              = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex   = 0;
    pipelineCreateInfo.flags               = 0;
    pipelineCreateInfo.pVertexInputState   = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState    = &colorBlendState;
    pipelineCreateInfo.pTessellationState  = nullptr;
    pipelineCreateInfo.pMultisampleState   = &multisampleState;
    pipelineCreateInfo.pDynamicState       = &dynamicState;
    pipelineCreateInfo.pViewportState      = &viewportState;
    pipelineCreateInfo.pDepthStencilState  = &depthStencilState;
    pipelineCreateInfo.pStages             = shaderStages.data();
    pipelineCreateInfo.stageCount          = shaderStages.size();
    pipelineCreateInfo.renderPass          = renderPass;
    pipelineCreateInfo.subpass             = 0;
    VK_CHECK(vkCreateGraphicsPipelines(
      device,
      pipelineCache,
      1,
      &pipelineCreateInfo,
      nullptr,
      &pipeline));
  }
  inline std::array<VkClearValue, 2> clearValues(const VkClearColorValue& clearColor               = {0, 0, 0, 1},
                                                 const VkClearDepthStencilValue& clearDepthStencil = { 1.f, 0 }) {
    std::array<VkClearValue, 2> clearValues;
    clearValues[0].color.float32[0]     = clearColor.float32[0];
    clearValues[0].color.float32[1]     = clearColor.float32[1];
    clearValues[0].color.float32[2]     = clearColor.float32[2];
    clearValues[0].color.float32[3]     = clearColor.float32[3];
    clearValues[1].depthStencil.depth   = clearDepthStencil.depth;
    clearValues[1].depthStencil.stencil = clearDepthStencil.stencil;
    return clearValues;
  }
  inline void beginRenderPass(const VkCommandBuffer& commandBuffer,
                              const VkRenderPass& renderPass,
                              const VkFramebuffer& framebuffer,
                              const VkExtent2D& renderArea,
                              const VkClearValue* clearValues) {
    VkRenderPassBeginInfo renderPassBegin;
    renderPassBegin.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBegin.pNext                    = nullptr;
    renderPassBegin.renderPass               = renderPass;
    renderPassBegin.framebuffer              = framebuffer;
    renderPassBegin.renderArea.offset.x      = 0;
    renderPassBegin.renderArea.offset.y      = 0;
    renderPassBegin.renderArea.extent.width  = renderArea.width;
    renderPassBegin.renderArea.extent.height = renderArea.height;
    renderPassBegin.clearValueCount          = 2;
    renderPassBegin.pClearValues             = clearValues;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
  }
  inline void submitCommandBuffer(VkQueue queue,
                                  uint32_t commandBufferCount,
                                  const VkCommandBuffer* commandBuffers,
                                  uint32_t waitSemaphoreCount,
                                  const VkSemaphore* waitSemaphores,
                                  const VkPipelineStageFlags* stageFlags,
                                  VkFence fence) {
    VkSubmitInfo submitInfo         = {};
    submitInfo.pNext                = nullptr;
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = waitSemaphoreCount;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = stageFlags;
    submitInfo.commandBufferCount   = commandBufferCount;
    submitInfo.pCommandBuffers      = commandBuffers;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores    = nullptr;
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence));
  }

  inline void presentSwapchain(VkQueue queue, const VkSwapchainKHR* swapchain, const uint32_t* imageIndex) {
    VkPresentInfoKHR present   = {};
    present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext              = nullptr;
    present.swapchainCount     = 1;
    present.pSwapchains        = swapchain;
    present.pImageIndices      = imageIndex;
    present.pWaitSemaphores    = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults           = nullptr;
    VK_CHECK_PRESENT_TOLERANT(vkQueuePresentKHR(queue, &present));
  }
  inline void prepareFrame(const VkDevice& device,
                           VkSwapchainKHR swapchain,
                           VkSemaphore semaphore,
                           uint32_t* imageIndex,
                           bool* obsolete) {
    VkResult res;
    VK_CHECK_PRESENT_TOLERANT(res = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, imageIndex));
    *obsolete = res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR;
  }
}