#include "vk_context.h"
#include "vk_wrap.h"

bool VkContext::bInit = false;

void VkContext::init(const char* appName, uint32_t appVersion, HWND windowHandle) {
  VkWrap::createInstance(*this, appName, appVersion);
  VkWrap::getDevices(*this);

  // Choose first available device
  physicalDevice = physicalDevices.at(0);
  VkWrap::printPhysicalDevice(physicalDevice);

  VkWrap::createDevice(*this);
  VkWrap::createSurface(*this, windowHandle);

  VkWrap::createSemaphore(*this, imageAcquiredSemaphore);
  VkWrap::createFence(*this, drawFence);

  // Compile shaders
  VkWrap::compileShaders(L".");

  // Remember that initialization was successful
  bInit = true;
}
