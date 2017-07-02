#pragma once

#include "pipeline.h"
#include "mvk_structs.h"
#include <glm\glm.hpp>
#include <vulkan\vulkan.hpp>

class MVkContext;
class SPH;

class MVkPipeline : public MPipeline {
public:
  MVkPipeline(MVkContext* context, SPH* sph) : context(context), sph(sph) { init(); }

  void render() override;

private:
  MVkContext*      context;
  MVKPipeline      pipeline;

  SPH* sph;

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

  void init() override;
  void initPipelineState();
  void initRenderPass();
  void initFramebuffers();
  void initStages();
  void initPipelineLayout();
  void initPipelineCache();
  void initPipeline();
  void initVertexBuffer();
  void initUniformBuffers();
  void updateDescriptorSet();
  void registerCommandBuffer();
};
