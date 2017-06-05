#pragma once

#include <vulkan\vulkan.hpp>
#include <Windows.h>

class VkContext {
public:
  // Get singleton instance of Vulkan context
  static VkContext* getContext(HWND windowHandle) {
    static VkContext context;
    if (!bInit) {
      context.init(windowHandle);
      bInit = true;
    }
    return &context;
  }

  // Render scene
  void render();

  // Instance is publicly accessible
  VkInstance instance;
  // Surface is publicly accessible
  VkSurfaceKHR surface;

private:
  VkContext() {}

  // Initialize context
  VkResult init(HWND windowHandle);

  // Has context been initialized?
  static bool bInit;

  // Actors of Vulkan context
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSwapchainKHR swapchain;
  VkSemaphore imageAcquiredSemaphore;
  VkFence drawFence;
  uint32_t currentBuffer;
  uint32_t vertexCount;
  VkCommandBuffer commandBuffer;
  VkRenderPass renderPass;
  VkExtent2D renderArea;
  VkPipeline pipeline;
  VkPipelineLayout pipelineLayout;
  VkQueue graphicsQueue;
  std::vector<VkDescriptorSet> descriptorSets;
  std::vector<VkBuffer> vertexBuffers;
  std::vector<VkFramebuffer> framebuffers;
  std::vector<VkViewport> viewports;
  std::vector<VkRect2D> scissors;
};
