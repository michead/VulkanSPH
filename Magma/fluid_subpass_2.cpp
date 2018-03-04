#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "fluid_subpass_2.h"
#include "gfx_wrap.h"
#include "gfx_utils.h"

void FluidSubpass2::init() {
  Subpass::init();

  description                      = MVkBaseColorSubpass;
  description.inputAttachmentCount = 1;
  description.pInputAttachments    = &MVkBaseInputAttachmentReference;

  dependency.srcSubpass      = index - 1;
  dependency.dstSubpass      = index;
  dependency.srcStageMask    = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependency.srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
}

void FluidSubpass2::postInit() {
  Subpass::postInit();
}

void FluidSubpass2::initGraphicsPipeline() {
  std::vector<VkVertexInputBindingDescription> bindings = shaderProgram.getVertexInputBindings();
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
  inputAssemblyState = MVkPipelineInputAssemblyStateTriangleList;
  rasterizationState = MVkPipelineRasterizationStateSPH;
  colorBlendState    = MVkPipelineColorBlendStateSPH;
  multisampleState   = MVkPipelineMultisampleStateSPH;
  dynamicState       = MVkPipelineDynamicStateSPH;
  viewportState      = GfxUtils::viewportState(&gfxContext->viewport, &gfxContext->scissor);
  depthStencilState  = MVkPipelineDepthStencilStateOff;

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

void FluidSubpass2::update() {
  Subpass::update();
}

void FluidSubpass2::initUniformBuffers() {
  uniformsFS.invProj = glm::inverse(scene->camera->getProjectionMatrix());

  GfxWrap::createStagingBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsFS,
    sizeof(MVkFrag2),
    &stagingUniformBufferFSDesc,
    &uniformBufferFSDesc);
}

void FluidSubpass2::updateUniformBuffers() {
  uniformsFS.invProj = glm::inverse(scene->camera->getProjectionMatrix());

  GfxWrap::updateBuffer(
    gfxContext->device,
    sizeof(MVkFrag2),
    &uniformsFS,
    stagingUniformBufferFSDesc.allocSize,
    stagingUniformBufferFSDesc.deviceMemory,
    &stagingUniformBufferFSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingFSBufferCmd,
    sizeof(MVkFrag2),
    gfxContext->graphicsQueue,
    stagingUniformBufferFSDesc.buffer,
    uniformBufferFSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    gfxContext->device,
    gfxContext->graphicsQueue,
    copyStagingFSBufferCmd);
}

void FluidSubpass2::updateDescriptorSets() {
  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView   = gfxContext->depthBuffer.imageView;
  imageInfo.sampler     = gfxContext->depthBuffer.sampler;

  GfxWrap::updateImageDescriptorSet(
    gfxContext->device,
    descriptorSet,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    0,
    imageInfo);

  GfxWrap::updateBufferDescriptorSet(
    gfxContext->device,
    descriptorSet,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    1,
    uniformBufferFSDesc.bufferInfo);
}
  