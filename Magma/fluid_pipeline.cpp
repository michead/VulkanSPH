#include "fluid_pipeline.h"
#include "fluid_gbuffer_subpass.h"
#include "fluid.h"
#include "gfx_context.h"
#include "gfx_wrap.h"
#include "gfx_structs.h"
#include "gfx_utils.h"
#include "subpass.h"
#include "scene.h"

FluidPipeline::FluidPipeline(GfxContext* context, Scene* scene, Fluid* elem) : Pipeline(context, scene, elem) {
  init();
}

void FluidPipeline::init() {
  subpasses.clear();
  subpasses.push_back(new FluidGBufferSubpass(context, &renderPass, elem, 0));

  std::for_each(subpasses.begin(), subpasses.end(), [] (Subpass* subpass) {
    subpass->init();
  });
  
  initRenderPass();
  initFramebuffers();
  initVertexBuffer();
}

void FluidPipeline::postInit() {
  std::for_each(subpasses.begin(), subpasses.end(), [] (Subpass* subpass) {
    subpass->postInit();
  });
  initCommandBuffers();
}

void FluidPipeline::initCommandBuffers() {
  drawCmds.clear();
  drawCmds.resize(context->swapchainImageCount);

  std::array<VkClearValue, 2> clearValues;
  GfxWrap::clearValues(clearValues);

  for (size_t i = 0; i < context->swapchainImageCount; i++) {
    GfxWrap::createCommandBuffers(context->device, context->commandPool, 1, &drawCmds[i]);
    
    GfxWrap::beginCommandBuffer(drawCmds[i]);
    GfxWrap::beginRenderPass(
      drawCmds[i],
      renderPass,
      framebuffers[i],
      context->swapchain.extent,
      clearValues.data());

    vkCmdSetViewport(drawCmds[i], 0, 1, &context->viewport);
    vkCmdSetScissor(drawCmds[i], 0, 1, &context->scissor);

    std::for_each(subpasses.begin(), subpasses.end(), [&] (Subpass* subpass) {
      subpass->bind(drawCmds[i]);

      VkDeviceSize offset = 0;
      vkCmdBindVertexBuffers(drawCmds[i], 0, 1, &vertexBuffer, &offset);
      vkCmdDraw(drawCmds[i], to_fluid(elem)->particleCount, 1, 0, 0);

      // TODO: Handle rendering to fullscreen quad
    });

    vkCmdEndRenderPass(drawCmds[i]);
    VK_CHECK(vkEndCommandBuffer(drawCmds[i]));
  }
}

void FluidPipeline::initRenderPass() {
  std::vector<VkSubpassDescription> subpassDescriptions;
  std::vector<VkSubpassDependency> subpassDependencies;

  std::for_each(subpasses.begin(), subpasses.end(), [&](Subpass* subpass) {
    subpassDescriptions.push_back(subpass->getSubpassDescription());
    subpassDependencies.push_back(subpass->getSubpassDependency());
  });

  GfxWrap::createRenderPass(
    context->device,
    attachments,
    subpassDescriptions,
    subpassDependencies,
    renderPass);
}

void FluidPipeline::initFramebuffers() {
  GfxWrap::createFramebuffers(
    context->device,
    renderPass,
    context->swapchain.imageViews,
    context->depthBuffer.imageView,
    context->swapchain.extent.width,
    context->swapchain.extent.height,
    framebuffers);
}

void FluidPipeline::initVertexBuffer() {
  size_t size = to_fluid(elem)->particleCount * sizeof(glm::vec4);

  GfxWrap::createBuffer(context->physicalDevice,
    context->device,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    to_fluid(elem)->positions,
    size,
    vertexBuffer,
    &vertexBufferDesc.allocSize,
    vertexBufferDesc.deviceMemory,
    &vertexBufferDesc.mappedMemory,
    &vertexBufferDesc.bufferInfo);
}

void FluidPipeline::update() {
  updateBuffers();
}

void FluidPipeline::updateBuffers() {
  GfxWrap::updateBuffer(
    context->device,
    to_fluid(elem)->particleCount * sizeof(glm::vec4),
    to_fluid(elem)->positions,
    vertexBufferDesc.allocSize,
    vertexBufferDesc.deviceMemory,
    &vertexBufferMappedMemory);
}
