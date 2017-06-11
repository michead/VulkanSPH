#pragma once

#include <vulkan\vulkan.hpp>
#include <Windows.h>

class VkContext {
public:
  // Get singleton instance of Vulkan context
  static VkContext* getContext(const char* appName, uint32_t appVersion, HWND windowHandle) {
    static VkContext context;
    if (!bInit) {
      context.init(appName, appVersion, windowHandle);
      bInit = true;
    }
    return &context;
  }

  // Publicly-accessible members
  VkInstance       instance;
  VkSurfaceKHR     surface;
  VkExtent2D       renderArea;
  VkPhysicalDevice physicalDevice;
  VkDevice         device;
  VkSwapchainKHR   swapchain;
  VkSemaphore      imageAcquiredSemaphore;
  VkFence          drawFence;
  uint32_t         currentSwapchainImageIndex;
  VkCommandPool    commandPool;
  VkCommandBuffer  commandBuffer;
  VkDescriptorPool descriptorPool;
  VkRenderPass     renderPass;
  uint32_t         graphicsQueueFamilyIndex;
  uint32_t         presentQueueFamilyIndex;
  uint32_t         swapchainImageCount;
  VkFormat         format;
  VkQueue          graphicsQueue;
  VkQueue          presentQueue;

  std::vector<const char*>             extensions;
  std::vector<VkImage>                 swapchainImages;
  std::vector<VkImageView>             swapchainImageViews;
  std::vector<VkViewport>              viewports;
  std::vector<VkRect2D>                scissors;
  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkPhysicalDevice>        physicalDevices;
  std::vector<VkQueueFamilyProperties> queueFamilyProps;

private:
  VkContext() {}

  // Initialize context
  void init(const char* appName, uint32_t appVersion, HWND windowHandle);

  // Has context been initialized?
  static bool bInit;
};
