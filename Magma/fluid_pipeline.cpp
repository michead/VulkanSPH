#include "fluid_pipeline.h"
#include "fluid_gbuffer_subpass.h"
#include "fluid.h"
#include "magma_context.h"
#include "gfx_wrap.h"
#include "gfx_structs.h"
#include "gfx_utils.h"
#include "subpass.h"
#include "scene.h"

FluidPipeline::FluidPipeline(const MagmaContext* context, Scene* scene, Fluid* elem) : Pipeline(context, scene, elem) {
  init();
}

void FluidPipeline::init() {
  subpasses.clear();
  subpasses.push_back(new FluidGBufferSubpass(context, &renderPass, elem, 0));

  std::for_each(subpasses.begin(), subpasses.end(), [] (Subpass* subpass) {
    subpass->init();
  });
  
  initRenderPass();
  initVertexBuffer();
}

void FluidPipeline::postInit() {
  std::for_each(subpasses.begin(), subpasses.end(), [] (Subpass* subpass) {
    subpass->postInit();
  });
}

void FluidPipeline::draw() {
  VkCommandBuffer cmdBuffer = context->graphics->getCurrentCmdBuffer();

  std::for_each(subpasses.begin(), subpasses.end(), [&] (Subpass* subpass) {
    subpass->bind(cmdBuffer);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, &offset);
    vkCmdDraw(cmdBuffer, to_fluid(elem)->particleCount, 1, 0, 0);

    // TODO: Handle rendering to fullscreen quad
  });
}

void FluidPipeline::initRenderPass() {
  std::vector<VkSubpassDescription> subpassDescriptions;
  std::vector<VkSubpassDependency> subpassDependencies;

  std::for_each(subpasses.begin(), subpasses.end(), [&](Subpass* subpass) {
    subpassDescriptions.push_back(subpass->getSubpassDescription());
    subpassDependencies.push_back(subpass->getSubpassDependency());
  });

  GfxWrap::createRenderPass(
    context->graphics->device,
    attachments,
    subpassDescriptions,
    subpassDependencies,
    renderPass);
}

void FluidPipeline::initVertexBuffer() {
  size_t size = to_fluid(elem)->particleCount * sizeof(glm::vec4);

  GfxWrap::createBuffer(context->graphics->physicalDevice,
    context->graphics->device,
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

  for each(const auto& subpass in subpasses) {
    subpass->update();
  }
}

void FluidPipeline::updateBuffers() {
  GfxWrap::updateBuffer(
    context->graphics->device,
    to_fluid(elem)->particleCount * sizeof(glm::vec4),
    to_fluid(elem)->positions,
    vertexBufferDesc.allocSize,
    vertexBufferDesc.deviceMemory,
    &vertexBufferMappedMemory);
}
