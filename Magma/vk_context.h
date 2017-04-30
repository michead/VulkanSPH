#pragma once

#include <vulkan\vulkan.hpp>
#include <Windows.h>

class VkContext {
public:
  // Get singleton instance of Vulkan context
  static VkContext* getContext() {
    static VkContext context;
    return &context;
  }

  // Initialize context
  VkResult init(HWND windowHandle);

  // Instance is publicly accessible
  VkInstance instance;
  // Surface is publicly accessible
  VkSurfaceKHR surface;

private:
  VkContext() {}

  // Has context been initialized?
  bool bInit;

  // Actors of Vulkan context
  VkPhysicalDevice physicalDevice;
  VkDevice device;
};
