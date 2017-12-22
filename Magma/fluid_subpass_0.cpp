#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "fluid_subpass_0.h"
#include "magma_context.h"
#include "fluid.h"
#include "scene.h"
#include "gfx_utils.h"

void FluidSubpass0::init() {
  Subpass::init();
}

void FluidSubpass0::postInit() {
  Subpass::postInit();
}

void FluidSubpass0::initUniformBuffers() {
  uniformsVS.view = scene->camera->getViewMatrix();
  uniformsVS.proj = scene->camera->getProjectionMatrix();
  uniformsVS.particleSize = to_fluid(elem)->radius * 1000;

  GfxWrap::createStagingBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsVS,
    sizeof(MVkVert0),
    &stagingUniformBufferVSDesc,
    &uniformBufferVSDesc);

  uniformsFS.particleSize = uniformsVS.particleSize;
  uniformsFS.ambientColor = glm::vec4(0.5f, 0.5f, 0.5, 1.f);
  uniformsFS.fluidDiffuse = glm::vec4(0.0f, 0.0f, 1.f, 1.f);
  uniformsFS.lightCount = scene->lights.size();
  std::vector<glm::vec4> lights;
  std::transform(scene->lights.begin(), scene->lights.end(), std::back_inserter(lights), [](Light* light) -> glm::vec4 {
    return glm::vec4(light->pos, 1);
  });
  memcpy(uniformsFS.lightPos, lights.data(), sizeof(glm::vec4) * lights.size());
  uniformsFS.proj = scene->camera->getProjectionMatrix();
  uniformsFS.invProj = glm::inverse(scene->camera->getProjectionMatrix());
  uniformsFS.viewport = {
    gfxContext->viewport.x,
    gfxContext->viewport.y,
    gfxContext->viewport.width,
    gfxContext->viewport.height };

  GfxWrap::createStagingBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsFS,
    sizeof(MVkFrag0),
    &stagingUniformBufferFSDesc,
    &uniformBufferFSDesc);
}

void FluidSubpass0::initGraphicsPipeline() {
  std::vector<VkVertexInputBindingDescription> bindings     = shaderProgram.getVertexInputBindings();
  std::vector<VkVertexInputAttributeDescription> attributes = shaderProgram.getVertexInputAttributes();

  vertexInputState = {
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    nullptr,
    0,
    bindings.size(),
    bindings.data(),
    attributes.size(),
    attributes.data()
  };
  inputAssemblyState = MVkPipelineInputAssemblyStatePointList;
  rasterizationState = MVkPipelineRasterizationStateSPH;
  colorBlendState = MVkPipelineColorBlendStateSPH;
  multisampleState = MVkPipelineMultisampleStateSPH;
  dynamicState = MVkPipelineDynamicStateSPH;
  viewportState = GfxUtils::viewportState(&gfxContext->viewport, &gfxContext->scissor);
  depthStencilState = MVkPipelineDepthStencilStateSPH;

  GfxWrap::createGraphicsPipeline(
    gfxContext->device,
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
    mvkPipeline->getRenderPass(),
    pipelineCache,
    index,
    pipeline);
}

void FluidSubpass0::initVertexBuffers() {
  GfxWrap::createBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    to_fluid(elem)->positions,
    to_fluid(elem)->particleCount * sizeof(glm::vec4),
    &vertexBufferDesc);
}

void FluidSubpass0::update() {
  Subpass::update();

  GfxWrap::updateBuffer(
    gfxContext->device,
    to_fluid(elem)->particleCount * sizeof(glm::vec4),
    to_fluid(elem)->positions,
    vertexBufferDesc.allocSize,
    vertexBufferDesc.deviceMemory,
    &vertexBufferDesc.mappedMemory);
}

void FluidSubpass0::updateDescriptorSets() {
  GfxWrap::updateBufferDescriptorSet(
    gfxContext->device,
    descriptorSet,
    0,
    uniformBufferVSDesc.bufferInfo);

  GfxWrap::updateBufferDescriptorSet(
    gfxContext->device,
    descriptorSet,
    1,
    uniformBufferFSDesc.bufferInfo);
}

void FluidSubpass0::updateUniformBuffers() {
  uniformsVS.view = scene->camera->getViewMatrix();
  uniformsVS.proj = scene->camera->getProjectionMatrix();

  GfxWrap::updateBuffer(
    gfxContext->device,
    sizeof(MVkVert0),
    &uniformsVS,
    stagingUniformBufferVSDesc.allocSize,
    stagingUniformBufferVSDesc.deviceMemory,
    &stagingUniformBufferVSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingVSBufferCmd,
    sizeof(MVkVert0),
    gfxContext->graphicsQueue,
    stagingUniformBufferVSDesc.buffer,
    uniformBufferVSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    gfxContext->device,
    gfxContext->graphicsQueue,
    copyStagingVSBufferCmd);

  std::vector<glm::vec4> lights;
  std::transform(scene->lights.begin(), scene->lights.end(), std::back_inserter(lights), [](Light* light) -> glm::vec4 {
    return glm::vec4(light->pos, 1);
  });
  memcpy(uniformsFS.lightPos, lights.data(), sizeof(glm::vec4) * lights.size());
  uniformsFS.lightCount = lights.size();
  uniformsFS.proj = uniformsVS.proj;
  uniformsFS.invProj = glm::inverse(uniformsVS.proj);
  uniformsFS.viewport = {
    gfxContext->viewport.x,
    gfxContext->viewport.y,
    gfxContext->viewport.width,
    gfxContext->viewport.height };

  GfxWrap::updateBuffer(
    gfxContext->device,
    sizeof(MVkFrag0),
    &uniformsFS,
    stagingUniformBufferFSDesc.allocSize,
    stagingUniformBufferFSDesc.deviceMemory,
    &stagingUniformBufferFSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingFSBufferCmd,
    sizeof(MVkFrag0),
    gfxContext->graphicsQueue,
    stagingUniformBufferFSDesc.buffer,
    uniformBufferFSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    gfxContext->device,
    gfxContext->graphicsQueue,
    copyStagingFSBufferCmd);
}
