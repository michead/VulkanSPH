#include "fluid_subpass_1.h"
#include "gfx_wrap.h"
#include "gfx_utils.h"

void FluidSubpass1::init() {
  Subpass::init();

  description = MVkBaseColorSubpass;
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

void FluidSubpass1::postInit() {
  Subpass::postInit();
}

void FluidSubpass1::initUniformBuffers() {
  Subpass::initUniformBuffers();
}

void FluidSubpass1::initGraphicsPipeline() {
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
  inputAssemblyState = MVkPipelineInputAssemblyStateTriangleList;
  rasterizationState = MVkPipelineRasterizationStateSPH;
  colorBlendState    = MVkPipelineColorBlendStateSPH;
  multisampleState   = MVkPipelineMultisampleStateSPH;
  dynamicState       = MVkPipelineDynamicStateSPH;
  viewportState      = GfxUtils::viewportState(&gfxContext->viewport, &gfxContext->scissor);
  depthStencilState  = MVkPipelineDepthStencilStateSPH;

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

void FluidSubpass1::update() {
  Subpass::update();
}

void FluidSubpass1::updateDescriptorSets() {
  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView   = gfxContext->depthBuffer.imageView;
  imageInfo.sampler     = gfxContext->depthBuffer.sampler;

  GfxWrap::updateImageDescriptorSet(
    gfxContext->device,
    descriptorSet,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    0,
    imageInfo);
}
