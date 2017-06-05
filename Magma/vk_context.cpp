#include "vk_context.h"
#include "vk_wrap.h"

bool VkContext::bInit = false;

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

  // Create semaphore
  VK_CHECK(VkWrap::createSemaphore(device, imageAcquiredSemaphore));

  // Create fence
  VK_CHECK(VkWrap::createFence(device, drawFence));

  // Remember that initialization was successful
  bInit = true;

  // Initialization is successfull if it reached this point
  return VK_SUCCESS;
}

void VkContext::render() {
  VK_CHECK(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentBuffer));

  VkWrap::beginRenderPass(commandBuffer, renderPass, framebuffers[currentBuffer], renderArea, VkWrap::clearValues());

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
  vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), { 0 });

  vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
  vkCmdSetScissor(commandBuffer, 0, scissors.size(), scissors.data());

  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  vkCmdEndRenderPass(commandBuffer);

 VK_CHECK(vkEndCommandBuffer(commandBuffer));
 VK_CHECK(VkWrap::submitCommandBuffer(graphicsQueue, commandBuffer, imageAcquiredSemaphore, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, drawFence));

 VkResult res;
 do {
   VK_CHECK((res = vkWaitForFences(device, 1, &drawFence, VK_TRUE, UINT64_MAX)));
 } while (res == VK_TIMEOUT);

 VkWrap::presentSwapchain(graphicsQueue, swapchain, currentBuffer);
}