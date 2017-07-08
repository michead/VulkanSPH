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

    vkCmdBindPipeline(drawCmds[i],
      VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);

    vkCmdBindDescriptorSets(drawCmds[i],
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline.layout, 0, 1,
      &pipeline.descriptorSet, 0, nullptr);

    std::vector<VkDeviceSize> offsets(vertexBuffers.size(), 0);
    vkCmdBindVertexBuffers(drawCmds[i], 0,
      1, vertexBuffers.data(), offsets.data());

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

  pipeline.vertexInputState   = MVkPipelineVertexInputStateSPH;
  pipeline.inputAssemblyState = MVkPipelineInputAssemblyStateSPH;
  pipeline.rasterizationState = MVkPipelineRasterizationStateSPH;
  pipeline.colorBlendState    = MVkPipelineColorBlendStateSPH;
  pipeline.multisampleState   = MVkPipelieMultisampleStateSPH;
  pipeline.dynamicState       = MVkPipelineDynamicStateSPH;
  pipeline.viewportState      = MVkUtils::viewportState(context->viewport, context->scissor);
  pipeline.depthStencilState  = MVkPipelineDepthStencilStateSPH;
}

void MVkPipeline::initRenderPass() {
  MVkWrap::createRenderPass(context->device, pipeline.handle, attachments, subpasses, pipeline.renderPass);
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
  vertexBuffers.clear();
  vertexBuffers.resize(1);
  vertexBufferMemoryVec.clear();
  vertexBufferMemoryVec.resize(1);
  vertexBufferMappedMemoryVec.clear();
  vertexBufferMappedMemoryVec.resize(1);

  size_t size = fluid->particleCount * sizeof(glm::vec4);

  VkDeviceSize allocSize;
  MVkWrap::createBuffer(context->physicalDevice,
                        context->device,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        fluid->positions,
                        size,
                        vertexBuffers[0],
                        &allocSize,
                        vertexBufferMemoryVec[0],
                        &vertexBufferMappedMemoryVec[0]);
}

void MVkPipeline::initUniformBuffers() {
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
  uniformsVS.mvp = camera->getMatrix();
  uniformsVS.particleSize = 2.f;

  uniformsFS.ambientColor = glm::vec4(0.5f, 0.5f, 0.5, 1.f);
  uniformsFS.fluidDiffuse = glm::vec4(0.f, 0.f, 1.f, 1.f);
  uniformsFS.lightCount = 0;
  uniformsFS.mvp = camera->getMatrix();
  uniformsFS.viewport = {
    context->viewport.x,
    context->viewport.y,
    context->viewport.width,
    context->viewport.height };

  memcpy(uniformBufferVS.mappedMemory, &uniformBufferVS, sizeof(MVkVertexShaderUniformParticle));
  memcpy(uniformBufferFS.mappedMemory, &uniformBufferFS, sizeof(MVkFragmentShaderUniformParticle));
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
  return drawCmds[context->currentSwapchainImageIndex];
}
