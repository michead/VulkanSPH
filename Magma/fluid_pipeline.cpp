#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "fluid_pipeline.h"
#include "gfx_context.h"
#include "gfx_wrap.h"
#include "gfx_structs.h"
#include "gfx_utils.h"
#include "scene.h"
#include "fluid.h"

FluidPipeline::FluidPipeline(GfxContext* context, Camera* camera, Fluid* elem) : Pipeline(context, camera, elem) {
  init();
}

void FluidPipeline::init() {
  initPipelineState();
  initRenderPass();
  initFramebuffers();
  initStages();
  initVertexBuffer();
  initPipelineLayout();
  initPipelineCache();
  initUniformBuffers();
  updateDescriptorSets();

  initPipeline();
  context->setPipeline(this);

  initCommandBuffers();
}

void FluidPipeline::initPipelineLayout() {
  std::vector<VkDescriptorSetLayoutBinding> bindings = {
    MVkDescriptorSetLayoutBindingUniformBufferVS,
    MVkDescriptorSetLayoutBindingUniformBufferFS,
  };
  GfxWrap::createDescriptorSetLayout(context->device, bindings, pipeline.descriptorSetLayout);
  GfxWrap::createDescriptorSet(context->device, context->descriptorPool, pipeline.descriptorSetLayout, pipeline.descriptorSet);
  GfxWrap::createPipelineLayout(context->device, 1, &pipeline.descriptorSetLayout, pipeline.layout);
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
      pipeline.renderPass,
      framebuffers[i],
      context->swapchain.extent,
      clearValues.data());

    vkCmdBindPipeline(drawCmds[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);

    vkCmdBindDescriptorSets(drawCmds[i],
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline.layout, 0, 1,
      &pipeline.descriptorSet, 0, nullptr);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(drawCmds[i], 0, 1, &vertexBuffer, &offset);

    vkCmdSetViewport(drawCmds[i], 0, 1, &context->viewport);
    vkCmdSetScissor(drawCmds[i], 0, 1, &context->scissor);

    vkCmdDraw(drawCmds[i], to_fluid(elem)->particleCount, 1, 0, 0);

    vkCmdEndRenderPass(drawCmds[i]);
    VK_CHECK(vkEndCommandBuffer(drawCmds[i]));
  }
}

void FluidPipeline::initPipelineState() {
  attachments.clear();
  attachments.push_back(MVkBaseAttachmentColor);
  attachments.push_back(MVkBaseAttachmentDepth);

  subpasses.clear();
  subpasses.push_back(MVkBaseSubpass);

  dependencies = MVkBaseDependencies;

  pipeline.vertexInputState = MVkPipelineVertexInputStateSPH;
  pipeline.inputAssemblyState = MVkPipelineInputAssemblyStateSPH;
  pipeline.rasterizationState = MVkPipelineRasterizationStateSPH;
  pipeline.colorBlendState = MVkPipelineColorBlendStateSPH;
  pipeline.multisampleState = MVkPipelineMultisampleStateSPH;
  pipeline.dynamicState = MVkPipelineDynamicStateSPH;
  pipeline.viewportState = GfxUtils::viewportState(&context->viewport, &context->scissor);
  pipeline.depthStencilState = MVkPipelineDepthStencilStateSPH;
}

void FluidPipeline::initRenderPass() {
  GfxWrap::createRenderPass(
    context->device,
    pipeline.handle,
    attachments,
    subpasses,
    dependencies,
    pipeline.renderPass);
}

void FluidPipeline::initFramebuffers() {
  GfxWrap::createFramebuffers(
    context->device,
    pipeline.renderPass,
    context->swapchain.imageViews,
    context->depthBuffer.imageView,
    context->swapchain.extent.width,
    context->swapchain.extent.height,
    framebuffers);
}

void FluidPipeline::initStages() {
  VkShaderModule moduleVert;
  VkShaderModule moduleFrag;

  VkPipelineShaderStageCreateInfo createInfoVert;
  VkPipelineShaderStageCreateInfo createInfoFrag;

  GfxWrap::createShaderModule(context->device, context->shaderMap["particle"]["vert"], moduleVert);
  GfxWrap::createShaderModule(context->device, context->shaderMap["particle"]["frag"], moduleFrag);

  GfxWrap::shaderStage(moduleVert, VK_SHADER_STAGE_VERTEX_BIT, createInfoVert);
  GfxWrap::shaderStage(moduleFrag, VK_SHADER_STAGE_FRAGMENT_BIT, createInfoFrag);

  pipeline.shaderStages.clear();
  pipeline.shaderStages.push_back(createInfoVert);
  pipeline.shaderStages.push_back(createInfoFrag);
}

void FluidPipeline::initPipelineCache() {
  pipeline.cache = VK_NULL_HANDLE;
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

void FluidPipeline::initUniformBuffers() {
  uniformsVS.view         = camera->getViewMatrix();
  uniformsVS.proj         = camera->getProjectionMatrix();
  uniformsVS.particleSize = to_fluid(elem)->radius;

  GfxWrap::createBuffer(context->physicalDevice,
    context->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsVS,
    sizeof(MVkVertexShaderUniformParticle),
    uniformBufferVSDesc.buffer,
    &uniformBufferVSDesc.allocSize,
    uniformBufferVSDesc.deviceMemory,
    &uniformBufferVSDesc.mappedMemory,
    &uniformBufferVSDesc.bufferInfo);

  uniformsFS.particleSize = 10.f;
  uniformsFS.ambientColor = glm::vec4(0.5f, 0.5f, 0.5, 1.f);
  uniformsFS.fluidDiffuse = glm::vec4(0.0f, 0.0f, 1.f, 1.f);
  uniformsFS.lightCount   = 0;
  uniformsFS.proj         = camera->getProjectionMatrix();
  uniformsFS.invProj      = glm::inverse(camera->getProjectionMatrix());
  uniformsFS.viewport = {
    context->viewport.x,
    context->viewport.y,
    context->viewport.width,
    context->viewport.height };

  GfxWrap::createBuffer(context->physicalDevice,
    context->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsFS,
    sizeof(MVkFragmentShaderUniformParticle),
    uniformBufferFSDesc.buffer,
    &uniformBufferFSDesc.allocSize,
    uniformBufferFSDesc.deviceMemory,
    &uniformBufferFSDesc.mappedMemory,
    &uniformBufferFSDesc.bufferInfo);
}

void FluidPipeline::update() {
  updateBuffers();
}

void FluidPipeline::updateDescriptorSets() {
  GfxWrap::updateDescriptorSet(context->device, pipeline.descriptorSet, 0, uniformBufferVSDesc.bufferInfo);
  GfxWrap::updateDescriptorSet(context->device, pipeline.descriptorSet, 1, uniformBufferFSDesc.bufferInfo);
}

void FluidPipeline::updateBuffers() {
  uniformsVS.view = camera->getViewMatrix();
  uniformsVS.proj = camera->getProjectionMatrix();

  GfxWrap::updateBuffer(
    context->device,
    sizeof(MVkVertexShaderUniformParticle),
    &uniformsVS,
    uniformBufferVSDesc.allocSize,
    uniformBufferFSDesc.deviceMemory,
    &uniformBufferFSDesc.mappedMemory);

  uniformsFS.proj = camera->getProjectionMatrix();
  uniformsFS.invProj = glm::inverse(camera->getProjectionMatrix());
  uniformsFS.viewport = {
    context->viewport.x,
    context->viewport.y,
    context->viewport.width,
    context->viewport.height };

  GfxWrap::updateBuffer(
    context->device,
    sizeof(MVkFragmentShaderUniformParticle),
    &uniformsFS,
    uniformBufferFSDesc.allocSize,
    uniformBufferFSDesc.deviceMemory,
    &uniformBufferFSDesc.mappedMemory);

  GfxWrap::updateBuffer(
    context->device,
    to_fluid(elem)->particleCount * sizeof(glm::vec4),
    to_fluid(elem)->positions,
    vertexBufferDesc.allocSize,
    vertexBufferDesc.deviceMemory,
    &vertexBufferMappedMemory);
}

void FluidPipeline::initPipeline() {
  GfxWrap::createGraphicsPipeline(
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
