#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include "magma.h"
#include "logger.h"

#define VK_VER_MAJOR(X)   ((((uint32_t)(X)) >> 22) & 0x3FF)
#define VK_VER_MINOR(X)   ((((uint32_t)(X)) >> 12) & 0x3FF)
#define VK_VER_PATCH(X)   (((uint32_t) (X))        & 0xFFF)
#define VK_CHECK(result)  ; // nop

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
}