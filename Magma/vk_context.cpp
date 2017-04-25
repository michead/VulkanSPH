#include "vk_context.h"

VkResult VkContext::init() {
  VK_CHECK(VkWrap::createInstance(instance));

  std::vector<VkPhysicalDevice> physicalDevices;
  VK_CHECK(VkWrap::getDevices(instance, physicalDevices));
  // Choose first available device
  physicalDevice = physicalDevices.at(0);
  VkWrap::printPhysicalDevice(physicalDevice);

  VK_CHECK(VkWrap::createDevice(physicalDevice, device));

  // Initialization is successfull if it reached this point
  return VK_SUCCESS;
}