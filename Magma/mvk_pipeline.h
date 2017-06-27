#pragma once

#include "pipeline.h"
#include "mvk_structs.h"
#include <glm\glm.hpp>
#include <vulkan\vulkan.hpp>

class MVkContext;

class MVkPipeline : public MPipeline {
public:
  MVkPipeline(MVkContext* context, MVkPipelineParams& params) : context(context) { init(params); }

  void render() override;

private:
  MVkContext*      context;
  MVKPipeline      pipeline;

  MVkVertexShaderUniformParticle   uniformsVS;
  MVkFragmentShaderUniformParticle uniformsFS;

  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkDescriptorSet>         descriptorSets;
  std::vector<VkBuffer>                vertexBuffers;
  std::vector<VkDeviceMemory>          vertexBufferMemoryVec;
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkSubpassDescription>    subpasses;
  std::vector<VkImageView>             colorAttachments;
  VkBuffer                             uniformBufferVS;
  VkBuffer                             uniformBufferFS;
  VkDescriptorBufferInfo               uniformBufferInfoVS;
  VkDescriptorBufferInfo               uniformBufferInfoFS;
  VkDeviceMemory                       uniformBufferMemoryVS;
  VkDeviceMemory                       uniformBufferMemoryFS;
  VkImageView                          depthAttachment;
  uint32_t                             vertexCount;

  void init(const MVkPipelineParams& params) override;
  void initPipelineState();
  void initRenderPass();
  void initFramebuffers();
  void initStages();
  void initPipelineLayout();
  void initPipelineCache();
  void initPipeline();
  void initVertexBuffer(uint32_t vertexCount, const glm::vec4* vertices);
  void initUniformBuffers();
  void updateDescriptorSet();
  void registerCommandBuffer();
};
