#include "subpass.h"
#include "gfx_structs.h"
#include "gfx_utils.h"
#include "gfx_wrap.h"

void Subpass::init() {
  description = MVkBaseSubpass;

  // Set dependency
  dependency.srcSubpass      = index > 0 ? index - 1 : VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass      = index > 0 ? index : 0;
  dependency.srcStageMask    = index > 0 ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependency.dstStageMask    = index > 0 ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask   = index > 0 ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : VK_ACCESS_MEMORY_READ_BIT;
  dependency.dstAccessMask   = index > 0 ? VK_ACCESS_SHADER_READ_BIT : VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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
  initGraphicsPipeline();
  initVertexBuffers();

  GfxWrap::createCommandBuffers(gfxContext->device, gfxContext->getCurrentCmdPool(), 1, &copyStagingVSBufferCmd);
  GfxWrap::createCommandBuffers(gfxContext->device, gfxContext->getCurrentCmdPool(), 1, &copyStagingFSBufferCmd);
}

void Subpass::initVertexBuffers() {
  GfxWrap::createBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    fsQuadVertices.data(),
    4 * (sizeof(glm::vec4) + sizeof(glm::vec2)),
    &vertexBufferDesc);

  GfxWrap::createBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    fsQuadIndices.data(),
    6 * sizeof(uint32_t),
    &indexBufferDesc);
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
