#include "mvk_context.h"
#include "mvk_wrap.h"
#include "magma.h"

bool MVkContext::bInit = false;

void MVkContext::init(const char* appName, uint32_t appVersion, HWND windowHandle) {
  initInstance(appName, appVersion);
  selectPhysicalDevice();
  initSurface(windowHandle);
  initDevice();
  initSwapchain();
  initDepthBuffer();
  MVkWrap::createSemaphore(device, imageAcquiredSemaphore);
  MVkWrap::createFence(device, drawFence);
  initCommandPool();
  initCommandBuffer();
  MVkWrap::compileShaders(SHADER_PATH);
  bInit = true;
}

void MVkContext::setValidationLayers() {
  validationLayers.clear();
#if defined(_DEBUG)
  validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
}

void MVkContext::initInstance(const char* appName, uint32_t appVersion) {
  MVkWrap::createInstance(appName, appVersion, instance);
}

void MVkContext::initDevice() {
  MVkWrap::queryQueueFamilyIndices(physicalDevice, surface, &graphicsQueueFamilyIndex, &presentQueueFamilyIndex);
  MVkWrap::createDevice(physicalDevice, graphicsQueueFamilyIndex, presentQueueFamilyIndex, device, graphicsQueue, presentQueue);
}
void MVkContext::selectPhysicalDevice() {
  MVkWrap::queryDevices(instance, physicalDevices);
  if (physicalDevices.empty()) {
    MVK_FAIL("No Vulkan-enabled GPUs found.");
  }
  physicalDevice = physicalDevices.at(0);
  MVkWrap::printDeviceStats(physicalDevice);
}

void MVkContext::initCommandPool() {
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.queueFamilyIndex = graphicsQueueFamilyIndex;
  info.flags = 0;
  VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &commandPool));
  VK_REGISTER(VkCommandPool, 1, &commandPool);
}

void MVkContext::initSurface(HWND hwnd) {
  MVkWrap::createSurface(instance, physicalDevice, hwnd,
    &graphicsQueueFamilyIndex, &presentQueueFamilyIndex,
    surface, surfaceCapabilities, format);
}

void MVkContext::initSwapchain() {
  MVkWrap::createSwapchain(device,
    surface, surfaceCapabilities, format,
    graphicsQueueFamilyIndex, presentQueueFamilyIndex, 
    swapchain.handle, swapchain.extent, swapchain.images, swapchain.imageViews);
  currentSwapchainImageIndex = 0;
}

void MVkContext::initDescriptorPool() {
  VkDescriptorPoolSize descriptorPoolSize = {
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    1
  };
  MVkWrap::createDescriptorPool(device, descriptorPoolSize, descriptorPool);
}


void MVkContext::initCommandBuffer() {
  MVkWrap::createCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void MVkContext::initDepthBuffer() {
  VkDeviceMemory deviceMemory;
  MVkWrap::createDepthBuffer(
    physicalDevice,
    device,
    swapchain.extent,
    VK_NUM_IMAGE_SAMPLES,
    depthBuffer.image,
    deviceMemory,
    depthBuffer.imageView);
}
