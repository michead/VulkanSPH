#include "vk_context.h"
#include "vk_wrap.h"

bool VkContext::bInit = false;

VkResult VkContext::init(HWND windowHandle) {
  // Create instance
  VkWrap::createInstance(instance);
  
  // Create physical device
  std::vector<VkPhysicalDevice> physicalDevices;
  VkWrap::getDevices(instance, physicalDevices);
  // Choose first available device
  physicalDevice = physicalDevices.at(0);
  VkWrap::printPhysicalDevice(physicalDevice);

  // Create logical device
  VkWrap::createDevice(physicalDevice, device);

  // Create surface
  VkWrap::createSurface(instance, windowHandle, surface);

  // Remember that initialization was successful
  bInit = true;

  // Initialization is successfull if it reached this point
  return VK_SUCCESS;
}