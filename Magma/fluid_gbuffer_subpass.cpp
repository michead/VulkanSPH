#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "fluid_gbuffer_subpass.h"
#include "magma_context.h"
#include "gfx_utils.h"
#include "fluid.h"
#include "scene.h"

void FluidGBufferSubpass::init() {
  Subpass::init();

  // Set dependency
  dependency.srcSubpass      = index > 0 ? index -1 : VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass      = index > 0 ? index : 0;
  dependency.srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
  dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  // Set shader program name
  shaderProgram = GfxUtils::buildShaderProgram(context->graphics, "fluid_gbuffer");

  // Set pipeline layout
  GfxWrap::createDescriptorSetLayout(context->graphics->device, shaderProgram.getLayoutBindings(), descriptorSetLayout);
  GfxWrap::createDescriptorSet(context->graphics->device, context->graphics->descriptorPool, descriptorSetLayout, descriptorSet);
  GfxWrap::createPipelineLayout(context->graphics->device, 1, &descriptorSetLayout, layout);
}

void FluidGBufferSubpass::postInit() {
  Scene* scene        = context->graphics->scene;
  VkDevice device     = context->graphics->device;
  VkViewport viewport = context->graphics->viewport;
  VkRect2D scissor    = context->graphics->scissor;

  // Init uniform buffers
  uniformsVS.view = scene->camera->getViewMatrix();
  uniformsVS.proj = scene->camera->getProjectionMatrix();
  uniformsVS.particleSize = to_fluid(elem)->radius * 1000;

  GfxWrap::createStagingBuffer(
    context->graphics->physicalDevice,
    context->graphics->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsVS,
    sizeof(MVkVertexShaderUniformParticle),
    &stagingUniformBufferVSDesc,
    &uniformBufferVSDesc);

  uniformsFS.particleSize = uniformsVS.particleSize;
  uniformsFS.ambientColor = glm::vec4(0.5f, 0.5f, 0.5, 1.f);
  uniformsFS.fluidDiffuse = glm::vec4(0.0f, 0.0f, 1.f, 1.f);
  uniformsFS.lightCount   = scene->lights.size();
  memcpy(uniformsFS.lights, scene->lights.data(), sizeof(Light) * uniformsFS.lightCount);
  uniformsFS.proj         = scene->camera->getProjectionMatrix();
  uniformsFS.invProj      = glm::inverse(scene->camera->getProjectionMatrix());
  uniformsFS.viewport = {
    viewport.x,
    viewport.y,
    viewport.width,
    viewport.height };

  GfxWrap::createStagingBuffer(
    context->graphics->physicalDevice,
    context->graphics->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsFS,
    sizeof(MVkFragmentShaderUniformParticle),
    &stagingUniformBufferFSDesc,
    &uniformBufferFSDesc);

  // Update descriptor sets
  GfxWrap::updateDescriptorSet(device, descriptorSet, 0, uniformBufferVSDesc.bufferInfo);
  GfxWrap::updateDescriptorSet(device, descriptorSet, 1, uniformBufferFSDesc.bufferInfo);

  // Init pipeline
  vertexInputState   = MVkPipelineVertexInputStateSPH;
  inputAssemblyState = MVkPipelineInputAssemblyStateSPH;
  rasterizationState = MVkPipelineRasterizationStateSPH;
  colorBlendState    = MVkPipelineColorBlendStateSPH;
  multisampleState   = MVkPipelineMultisampleStateSPH;
  dynamicState       = MVkPipelineDynamicStateSPH;
  viewportState      = GfxUtils::viewportState(&viewport, &scissor);
  depthStencilState  = MVkPipelineDepthStencilStateSPH;

  GfxWrap::createGraphicsPipeline(
    device,
    layout,
    vertexInputState,
    inputAssemblyState,
    rasterizationState,
    colorBlendState,
    multisampleState,
    dynamicState,
    viewportState,
    depthStencilState,
    shaderProgram.getStages(),
    *renderPass,
    pipelineCache,
    pipeline);


  GfxWrap::createCommandBuffers(device, context->graphics->getCurrentCmdPool(), 1, &copyStagingVSBufferCmd);
  GfxWrap::createCommandBuffers(device, context->graphics->getCurrentCmdPool(), 1, &copyStagingFSBufferCmd);
}

void FluidGBufferSubpass::update() {
  Scene* scene        = context->graphics->scene;
  VkDevice device     = context->graphics->device;
  VkViewport viewport = context->graphics->viewport;

  uniformsVS.view = scene->camera->getViewMatrix();
  uniformsVS.proj = scene->camera->getProjectionMatrix();

  GfxWrap::updateBuffer(
    device,
    sizeof(MVkVertexShaderUniformParticle),
    &uniformsVS,
    stagingUniformBufferVSDesc.allocSize,
    stagingUniformBufferVSDesc.deviceMemory,
    &stagingUniformBufferVSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingVSBufferCmd,
    sizeof(MVkVertexShaderUniformParticle),
    context->graphics->graphicsQueue,
    stagingUniformBufferVSDesc.buffer,
    uniformBufferVSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    context->graphics->device,
    context->graphics->graphicsQueue,
    copyStagingVSBufferCmd);

  std::vector<Light> lights;
  std::transform(scene->lights.begin(), scene->lights.end(), std::back_inserter(lights), [](Light* light) {
    return *light;
  });
  memcpy(&uniformsFS, lights.data(), sizeof(Light) * lights.size());
  uniformsFS.lightCount = lights.size();
  uniformsFS.proj     = uniformsVS.proj;
  uniformsFS.invProj  = glm::inverse(uniformsVS.proj);
  uniformsFS.viewport = {
    viewport.x,
    viewport.y,
    viewport.width,
    viewport.height };

  GfxWrap::updateBuffer(
    device,
    sizeof(MVkFragmentShaderUniformParticle),
    &uniformsFS,
    stagingUniformBufferFSDesc.allocSize,
    stagingUniformBufferFSDesc.deviceMemory,
    &stagingUniformBufferFSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingFSBufferCmd,
    sizeof(MVkFragmentShaderUniformParticle),
    context->graphics->graphicsQueue,
    stagingUniformBufferFSDesc.buffer,
    uniformBufferFSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    context->graphics->device,
    context->graphics->graphicsQueue,
    copyStagingFSBufferCmd);
}
