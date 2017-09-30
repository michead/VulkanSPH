#pragma once
#include <vulkan\vulkan.hpp>
#include <vector>
#include "magma_context.h"
#include "gfx_structs.h"
#include "scene_element.h"

class Subpass {
public:
  Subpass(const MagmaContext* context, VkRenderPass *renderPass, SceneElement* elem, uint8_t index) :
    context(context), renderPass(renderPass), elem(elem), index(index) {
    init();
  }

  virtual void init();
  virtual void postInit() {}
  virtual void update() {}
  virtual void bind(VkCommandBuffer cmd);

  VkSubpassDescription getSubpassDescription() {
    return description;
  }

  VkSubpassDependency getSubpassDependency() {
    return dependency;
  }

protected:
  const MagmaContext*          context;
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
  MVkBufferDesc                uniformBufferVSDesc;
  MVkBufferDesc                uniformBufferGSDesc;
  MVkBufferDesc                uniformBufferFSDesc;
  uint8_t                      index;

  VkPipelineVertexInputStateCreateInfo   vertexInputState;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
  VkPipelineRasterizationStateCreateInfo rasterizationState;
  VkPipelineColorBlendStateCreateInfo    colorBlendState;
  VkPipelineMultisampleStateCreateInfo   multisampleState;
  VkPipelineDynamicStateCreateInfo       dynamicState;
  VkPipelineViewportStateCreateInfo      viewportState;
  VkPipelineDepthStencilStateCreateInfo  depthStencilState;
};
