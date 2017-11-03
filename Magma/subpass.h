#pragma once
#include <vulkan\vulkan.hpp>
#include <vector>
#include "magma_context.h"
#include "gfx_structs.h"
#include "scene_element.h"
#include "pipeline.h"


class Subpass {
public:
  Subpass(const Pipeline* pipeline, const char* shaderName, uint8_t index, SceneElement* elem) :
      gfxContext(pipeline->getContext()->graphics), mvkPipeline(pipeline), shaderName(shaderName), index(index), elem(elem) {
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
  GfxContext*                  gfxContext;
  const Pipeline*              mvkPipeline;
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
