#pragma once
#include <vulkan\vulkan.hpp>
#include <vector>
#include "magma_context.h"
#include "gfx_structs.h"
#include "scene_element.h"

class Subpass {
public:
  Subpass(const MagmaContext* context, VkRenderPass *renderPass, SceneElement* elem, const char* shaderName, uint8_t index) :
    context(context), renderPass(renderPass), elem(elem), shaderName(shaderName), index(index) {
    init();
  }

  virtual void init();
  virtual void postInit();
  virtual void initUniformBuffers() {}
  virtual void update();
  virtual void updateDescriptorSets() {}
  virtual void updateUniformBuffers() {}
  virtual void bind(VkCommandBuffer cmd);

  VkSubpassDescription getSubpassDescription() {
    return description;
  }

  VkSubpassDependency getSubpassDependency() {
    return dependency;
  }

protected:
  const MagmaContext*          context;
  VkDevice                     device;
  VkViewport                   viewport;
  VkRect2D                     scissor;
  VkRenderPass*                renderPass;
  SceneElement*                elem;
  VkSubpassDescription         description;
  VkSubpassDependency          dependency;
  VkPipelineLayout             layout;
  VkDescriptorSet              descriptorSet;
  VkDescriptorSetLayout        descriptorSetLayout;
  VkPipelineLayout             pipelineLayout;
  std::vector<VkDescriptorSet> writeDesciptorSets;
  VkPipeline                   pipeline;
  VkPipelineCache              pipelineCache = VK_NULL_HANDLE;
  MVkShaderProgram             shaderProgram;
  MVkBufferDesc                stagingUniformBufferVSDesc;
  MVkBufferDesc                stagingUniformBufferGSDesc;
  MVkBufferDesc                stagingUniformBufferFSDesc;
  MVkBufferDesc                uniformBufferVSDesc;
  MVkBufferDesc                uniformBufferGSDesc;
  MVkBufferDesc                uniformBufferFSDesc;
  VkCommandBuffer              copyStagingVSBufferCmd;
  VkCommandBuffer              copyStagingFSBufferCmd;
  const char *                 shaderName;
  uint8_t                      index;
  Scene*                       scene;

  VkPipelineVertexInputStateCreateInfo   vertexInputState;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
  VkPipelineRasterizationStateCreateInfo rasterizationState;
  VkPipelineColorBlendStateCreateInfo    colorBlendState;
  VkPipelineMultisampleStateCreateInfo   multisampleState;
  VkPipelineDynamicStateCreateInfo       dynamicState;
  VkPipelineViewportStateCreateInfo      viewportState;
  VkPipelineDepthStencilStateCreateInfo  depthStencilState;
};
