#pragma once

#include <vulkan\vulkan.hpp>
#include "mvk_structs.h"
#include <Windows.h>

#define VK_VER_MAJOR(X)   ((((uint32_t)(X)) >> 22) & 0x3FF)
#define VK_VER_MINOR(X)   ((((uint32_t)(X)) >> 12) & 0x3FF)
#define VK_VER_PATCH(X)   ((( uint32_t)(X))        & 0xFFF)
#define VK_CHECK(result)  assert(result == VK_SUCCESS);
#define VK_ASSERT(handle) assert(handler != VK_NULL_HANDLE);
#define VK_REGISTER(type, count, instances) ;

#define VK_DESCRIPTOR_POOL_MAX_SETS 30
#define VK_DESCRIPTOR_POOL_SIZE     30
#define VK_NUM_IMAGE_SAMPLES        VK_SAMPLE_COUNT_1_BIT

#ifndef SHADER_PATH
#define SHADER_PATH L"."
#endif

class MVkContext {
public:
  // Get singleton instance of Vulkan context
  static MVkContext* getContext(const char* appName, uint32_t appVersion, HWND windowHandle) {
    static MVkContext context;
    if (!bInit) {
      context.init(appName, appVersion, windowHandle);
      bInit = true;
    }
    return &context;
  }

  VkInstance               instance;
  VkSurfaceKHR             surface;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  VkPhysicalDevice         physicalDevice;
  VkDevice                 device;
  VkSemaphore              imageAcquiredSemaphore;
  VkFence                  drawFence;
  uint32_t                 currentSwapchainImageIndex;
  VkCommandPool            commandPool;
  VkCommandBuffer          commandBuffer;
  VkDescriptorPool         descriptorPool;
  uint32_t                 graphicsQueueFamilyIndex;
  uint32_t                 presentQueueFamilyIndex;
  uint32_t                 swapchainImageCount;
  VkFormat                 format;
  VkQueue                  graphicsQueue;
  VkQueue                  presentQueue;
  VkViewport               viewport;
  VkRect2D                 scissor;

  MVkAttachment            depthBuffer;
  MVkSwapchain             swapchain;
  MVkShaderMap             shaderMap;

  std::vector<const char*>             extensions;
  std::vector<VkPhysicalDevice>        physicalDevices;
  std::vector<VkQueueFamilyProperties> queueFamilyProps;
  std::vector<const char*>             validationLayers;

private:
  MVkContext() {}

  // Initialize context
  void init(const char* appName, uint32_t appVersion, HWND windowHandle);

  void setValidationLayers();
  void initInstance(const char* appName, uint32_t appVersion);
  void selectPhysicalDevice();
  void initDevice();
  void initCommandPool();
  void initCommandBuffer();
  void initSurface(HWND hwnd);
  void initSwapchain();
  void initDepthBuffer();
  void initDescriptorPool();
  void initViewport();
  void loadShaders();

  // Has context been initialized?
  static bool bInit;
};
