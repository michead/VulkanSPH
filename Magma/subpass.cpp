#include "subpass.h"
#include "gfx_structs.h"
#include "gfx_utils.h"
#include "gfx_wrap.h"

void Subpass::init() {
  description = MVkBaseSubpass;

  // Set dependency
  dependency.srcSubpass = index > 0 ? index - 1 : VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = index > 0 ? index : 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  // Set shader program name
  shaderProgram = GfxUtils::buildShaderProgram(gfxContext, shaderName);

  // Set pipeline layout
  GfxWrap::createDescriptorSetLayout(gfxContext->device, shaderProgram.getLayoutBindings(), descriptorSetLayout);
  GfxWrap::createDescriptorSet(gfxContext->device, gfxContext->descriptorPool, descriptorSetLayout, descriptorSet);
  GfxWrap::createPipelineLayout(gfxContext->device, 1, &descriptorSetLayout, layout);
}

void Subpass::postInit() {
  scene = gfxContext->scene;

  initUniformBuffers();
  updateDescriptorSets();

  std::vector<VkVertexInputBindingDescription> bindings = shaderProgram.getVertexInputBindings();
  std::vector<VkVertexInputAttributeDescription> attributes = shaderProgram.getVertexInputAttributes();

  // Init pipeline
  vertexInputState = {
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    nullptr,
    0,
    bindings.size(),
    bindings.data(),
    attributes.size(),
    attributes.data()
  };
  inputAssemblyState = MVkPipelineInputAssemblyStateSPH;
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

  GfxWrap::createCommandBuffers(gfxContext->device, gfxContext->getCurrentCmdPool(), 1, &copyStagingVSBufferCmd);
  GfxWrap::createCommandBuffers(gfxContext->device, gfxContext->getCurrentCmdPool(), 1, &copyStagingFSBufferCmd);
}

void Subpass::update() {
  updateUniformBuffers();
}

void Subpass::bind(VkCommandBuffer cmd) {
  // Advance subpass if not first subpass
  if (index > 0) {
    vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);
  }
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet, 0, NULL);
}
