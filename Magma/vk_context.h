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
};
