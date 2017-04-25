#pragma once

#include "vk_wrap.h"


class VkContext {
public:
  // Get singleton instance of Vulkan context
  static VkContext& getContext() {
    static VkContext context;
    return context;
  }

  // Initialize context
  VkResult init();

private:
  VkContext() {}

  // Make sure not to have copies of the singleton instance
  VkContext     (VkContext const&) = delete;
  void operator=(VkContext const&) = delete;

  // Has context been initialized?
  bool bInit;

  // Actors of Vulkan context
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkInstance instance;
};
