#include "vk_path.h"
#include "vk_wrap.h"
#include "vk_context.h"

void VkPath::initCommandBuffer() {
  std::array<VkClearValue, 2> clearValues;
  VkWrap::clearValues(clearValues);
  VkWrap::beginRenderPass(commandBuffer, renderPass, framebuffer, context->renderArea, clearValues.data());
  vkCmdBindPipeline      (commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
  vkCmdBindVertexBuffers (commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), { 0 });
  vkCmdSetViewport(commandBuffer, 0, context->viewports.size(), context->viewports.data());
  vkCmdSetScissor (commandBuffer, 0, context->scissors.size(),  context->scissors.data());
  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  vkCmdEndRenderPass(commandBuffer);
  VK_CHECK(vkEndCommandBuffer(commandBuffer));
}
