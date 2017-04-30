#include "vk_context.h"
#include "vk_wrap.h"

VkResult VkContext::init(HWND windowHandle) {
  // Create instance
  VK_CHECK(VkWrap::createInstance(instance));

  // Create physical device
  std::vector<VkPhysicalDevice> physicalDevices;
  VK_CHECK(VkWrap::getDevices(instance, physicalDevices));
  // Choose first available device
  physicalDevice = physicalDevices.at(0);
  VkWrap::printPhysicalDevice(physicalDevice);

  // Create logical device
  VK_CHECK(VkWrap::createDevice(physicalDevice, device));

  // Create surface
  VK_CHECK(VkWrap::createSurface(instance, windowHandle, surface));

  // Initialization is successfull if it reached this point
  return VK_SUCCESS;
}