#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "mvk_pipeline.h"
#include "mvk_context.h"
#include "mvk_wrap.h"
#include "mvk_structs.h"
#include "mvk_utils.h"
#include "scene.h"

void MVkPipeline::init() {
  initPipelineState();
  initRenderPass();
  initFramebuffers();
  initStages();
  initVertexBuffer();
  initPipelineLayout();
  initPipelineCache();
  initPipeline();
  initUniformBuffers();
  updateDescriptorSets();
  initCommandBuffers();

  context->setPipeline(this);
}

void MVkPipeline::initPipelineLayout() {
  std::vector<VkDescriptorSetLayoutBinding> bindings = {
    MVkDescriptorSetLayoutBindingUniformBufferVS,
    MVkDescriptorSetLayoutBindingUniformBufferFS,
  };
  MVkWrap::createDescriptorSetLayout(context->device, bindings, pipeline.descriptorSetLayout);
  MVkWrap::createDescriptorSet(context->device, context->descriptorPool, pipeline.descriptorSetLayout, pipeline.descriptorSet);
  MVkWrap::createPipelineLayout(context->device, 1, &pipeline.descriptorSetLayout, pipeline.layout);
}

void MVkPipeline::initCommandBuffers() {
  drawCmds.clear();
  drawCmds.resize(context->swapchainImageCount);

  std::array<VkClearValue, 2> clearValues;
  MVkWrap::clearValues(clearValues);

  for (size_t i = 0; i < context->swapchainImageCount; i++) {
    MVkWrap::createCommandBuffers(context->device, context->commandPool, 1, &drawCmds[i]);
    
    MVkWrap::beginCommandBuffer(drawCmds[i]);
    MVkWrap::beginRenderPass(
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

    vkCmdDraw(drawCmds[i], fluid->particleCount, 1, 0, 0);

    vkCmdEndRenderPass(drawCmds[i]);
    VK_CHECK(vkEndCommandBuffer(drawCmds[i]));
  }
}

void MVkPipeline::initPipelineState() {
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
  pipeline.viewportState = MVkUtils::viewportState(&context->viewport, &context->scissor);
  pipeline.depthStencilState = MVkPipelineDepthStencilStateSPH;
}

void MVkPipeline::initRenderPass() {
  MVkWrap::createRenderPass(
    context->device,
    pipeline.handle,
    attachments,
    subpasses,
    dependencies,
    pipeline.renderPass);
}

void MVkPipeline::initFramebuffers() {
  MVkWrap::createFramebuffers(
    context->device,
    pipeline.renderPass,
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
  MVkWrap::createShaderModule(context->device, context->shaderMap["particle"]["frag"], moduleFrag);

  MVkWrap::shaderStage(moduleVert, VK_SHADER_STAGE_VERTEX_BIT, createInfoVert);
  MVkWrap::shaderStage(moduleFrag, VK_SHADER_STAGE_FRAGMENT_BIT, createInfoFrag);

  pipeline.shaderStages.clear();
  pipeline.shaderStages.push_back(createInfoVert);
  pipeline.shaderStages.push_back(createInfoFrag);
}

void MVkPipeline::initPipelineCache() {
  pipeline.cache = VK_NULL_HANDLE;
}

void MVkPipeline::initVertexBuffer() {
  size_t size = fluid->particleCount * sizeof(glm::vec4);

  VkDeviceSize allocSize;
  VkDescriptorBufferInfo vertexBufferInfo;
  MVkWrap::createBuffer(context->physicalDevice,
    context->device,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    fluid->positions,
    size,
    vertexBuffer,
    &allocSize,
    vertexBufferMemory,
    &vertexBufferMappedMemory,
    &vertexBufferInfo);
}

void MVkPipeline::initUniformBuffers() {
  uniformsVS.view         = camera->getViewMatrix();
  uniformsVS.proj         = camera->getProjectionMatrix();
  uniformsVS.particleSize = 10.f;

  MVkWrap::createBuffer(context->physicalDevice,
    context->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsVS,
    sizeof(MVkVertexShaderUniformParticle),
    uniformBufferVS.buffer,
    &uniformBufferVS.allocSize,
    uniformBufferVS.deviceMemory,
    &uniformBufferVS.mappedMemory,
    &uniformBufferVS.bufferInfo);

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

  MVkWrap::createBuffer(context->physicalDevice,
    context->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsFS,
    sizeof(MVkFragmentShaderUniformParticle),
    uniformBufferFS.buffer,
    &uniformBufferFS.allocSize,
    uniformBufferFS.deviceMemory,
    &uniformBufferFS.mappedMemory,
    &uniformBufferFS.bufferInfo);
}

void MVkPipeline::update() {
  updateBuffers();
}

void MVkPipeline::updateDescriptorSets() {
  MVkWrap::updateDescriptorSet(context->device, pipeline.descriptorSet, 0, uniformBufferVS.bufferInfo);
  MVkWrap::updateDescriptorSet(context->device, pipeline.descriptorSet, 1, uniformBufferFS.bufferInfo);
}

void MVkPipeline::updateBuffers() {
  uniformsVS.view = camera->getViewMatrix();
  uniformsVS.proj = camera->getProjectionMatrix();

  MVkWrap::updateBuffer(
    context->device,
    sizeof(MVkVertexShaderUniformParticle),
    &uniformsVS,
    uniformBufferVS.allocSize,
    uniformBufferFS.deviceMemory,
    &uniformBufferFS.mappedMemory);

  uniformsFS.proj = camera->getProjectionMatrix();
  uniformsFS.invProj = glm::inverse(camera->getProjectionMatrix());
  uniformsFS.viewport = {
    context->viewport.x,
    context->viewport.y,
    context->viewport.width,
    context->viewport.height };

  MVkWrap::updateBuffer(
    context->device,
    sizeof(MVkFragmentShaderUniformParticle),
    &uniformsFS,
    uniformBufferFS.allocSize,
    uniformBufferFS.deviceMemory,
    &uniformBufferFS.mappedMemory);
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

VkCommandBuffer MVkPipeline::getDrawCmdBuffer() const {
  return drawCmds[context->currentImageIndex];
}
