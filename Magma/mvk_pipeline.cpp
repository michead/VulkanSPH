#include "mvk_pipeline.h"
#include "mvk_context.h"
#include "mvk_wrap.h"

void MVkPipeline::init(const MVkPipelineParams& params) {
  initSubpasses();
  initRenderPass();
  initFramebuffers();
  initStages();
  initPipeline();
  registerCommandBuffer();
}

void MVkPipeline::render() {
  MVkWrap::prepareFrame(
    context->device,
    context->swapchain.handle,
    context->imageAcquiredSemaphore,
    context->currentSwapchainImageIndex);

  MVkWrap::submitCommandBuffer(
    context->graphicsQueue,
    context->commandBuffer,
    context->imageAcquiredSemaphore,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    context->drawFence);

  VkResult res;
  do {
    VK_CHECK((res = vkWaitForFences(context->device, 1, &context->drawFence, VK_TRUE, UINT64_MAX)));
  } while (res == VK_TIMEOUT);

  MVkWrap::presentSwapchain(
    context->presentQueue,
    context->swapchain.handle,
    context->currentSwapchainImageIndex);
}

void MVkPipeline::registerCommandBuffer() {
  std::array<VkClearValue, 2> clearValues;
  MVkWrap::clearValues(clearValues);
  MVkWrap::beginCommandBuffer(context->commandBuffer);
  MVkWrap::beginRenderPass(
    context->commandBuffer,
    renderPass,
    framebuffers[context->currentSwapchainImageIndex],
    context->swapchain.extent,
    clearValues.data());

  vkCmdBindPipeline(context->commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
  
  vkCmdBindDescriptorSets(context->commandBuffer, 
    VK_PIPELINE_BIND_POINT_GRAPHICS, 
    pipelineLayout, 0, 1,
    descriptorSets.data(), 0, nullptr);

  vkCmdBindVertexBuffers(context->commandBuffer, 0, 
    vertexBuffers.size(), vertexBuffers.data(), { 0 });

  vkCmdSetViewport(context->commandBuffer, 0,
    context->viewports.size(), context->viewports.data());
  
  vkCmdSetScissor(context->commandBuffer, 0,
    context->scissors.size(), context->scissors.data());
  
  vkCmdDraw(context->commandBuffer, vertexCount, 1, 0, 0);
  
  vkCmdEndRenderPass(context->commandBuffer);
  VK_CHECK(vkEndCommandBuffer(context->commandBuffer));
}

void MVkPipeline::initSubpasses() {
  attachments.clear();
  attachments.push_back(MVkBaseAttachmentColor);
  attachments.push_back(MVkBaseAttachmentDepth);

  subpasses.clear();
  subpasses.push_back(MVkBaseSubpass);
}

void MVkPipeline::initRenderPass() {
  MVkWrap::createRenderPass(context->device, pipeline.handle, attachments, subpasses, renderPass);
}

void MVkPipeline::initFramebuffers() {
  MVkWrap::createFramebuffer(
    context->device,
    renderPass,
    context->swapchain.imageViews,
    context->depthBuffer.imageView,
    context->swapchain.extent.width,
    context->swapchain.extent.height,
    framebuffers);
}

void MVkPipeline::initStages() {
  VkShaderModule moduleVert;
  VkShaderModule moduleFrag;

  VkPipelineShaderStageCreateInfo createInfoVert;
  VkPipelineShaderStageCreateInfo createInfoFrag;

  MVkWrap::createShaderModule(context->device, context->shaderMap["particle"]["vert"], moduleVert);
  MVkWrap::createShaderModule(context->device, context->shaderMap["particle"]["frag"], moduleVert);

  MVkWrap::shaderStage(moduleVert, VK_SHADER_STAGE_VERTEX_BIT, createInfoVert);
  MVkWrap::shaderStage(moduleFrag, VK_SHADER_STAGE_FRAGMENT_BIT, createInfoFrag);

  pipeline.shaderStages.push_back(createInfoVert);
  pipeline.shaderStages.push_back(createInfoFrag);
}

void MVkPipeline::initPipeline() {
  MVkWrap::createGraphicsPipeline(
    context->device,
    pipeline.layout,
    pipeline.vertexInputState,
    pipeline.inputAssemblyState,
    pipeline.rasterizationState,
    pipeline.colorBlendState,
    pipeline.multisampleState,
    pipeline.dynamicState,
    pipeline.viewportState,
    pipeline.depthStencilState,
    pipeline.shaderStages,
    pipeline.renderPass,
    pipeline.cache,
    pipeline.handle);
}
