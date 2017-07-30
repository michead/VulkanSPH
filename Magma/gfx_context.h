#pragma once

#include <Windows.h>
#include <vulkan\vulkan.hpp>
#include "gfx_structs.h"

#define VK_VER_MAJOR(X)   ((((uint32_t)(X)) >> 22) & 0x3FF)
#define VK_VER_MINOR(X)   ((((uint32_t)(X)) >> 12) & 0x3FF)
#define VK_VER_PATCH(X)   ((( uint32_t)(X))        & 0xFFF)

#define VK_DESCRIPTOR_POOL_MAX_SETS   30
#define VK_DESCRIPTOR_POOL_SIZE_COUNT 30
#define VK_DESCRIPTOR_POOL_SIZE       { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_POOL_SIZE_COUNT }
#define VK_NUM_IMAGE_SAMPLES          VK_SAMPLE_COUNT_1_BIT

#ifndef SHADER_PATH
#define SHADER_PATH L"."
#endif

struct Config;
class Pipeline;

class GfxContext {
public:
  // Get singleton instance of Vulkan context
  static GfxContext* getContext(const char* appName, uint32_t appVersion, HWND windowHandle, const Config* config) {
    static GfxContext context;
    if (!bInit) {
      context.init(appName, appVersion, windowHandle, config);
      bInit = true;
    }
    return &context;
  }

  void setPipeline(const Pipeline* pipeline) {
    this->pipeline = pipeline;
  }

  VkRenderPass getRenderPass() const;
  VkPipelineCache getPipelineCache() const;

  VkInstance               instance;
  VkSurfaceKHR             surface;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  VkPhysicalDevice         physicalDevice;
  VkDevice                 device;
  VkSemaphore              imageAcquiredSemaphore;
  uint32_t                 currentImageIndex;
  VkCommandPool            commandPool;
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

  const Pipeline*          pipeline;

  std::vector<const char*>             extensions;
  std::vector<VkPhysicalDevice>        physicalDevices;
  std::vector<VkQueueFamilyProperties> queueFamilyProps;
  std::vector<const char*>             validationLayers;
  std::vector<VkFence>                 drawFences;

private:
  GfxContext() {}

  // Initialize context
  void init(const char* appName, uint32_t appVersion, HWND windowHandle, const Config* config);

  void initInstance(const char* appName, uint32_t appVersion);
  void selectPhysicalDevice();
  void initDevice();
  void initCommandPool();
  void initSurface(HWND hwnd);
  void initSwapchain(glm::ivec2 resolution);
  void initDepthBuffer();
  void initDescriptorPool();
  void initViewport();
  void loadShaders();

  // Has context been initialized?
  static bool bInit;
};
