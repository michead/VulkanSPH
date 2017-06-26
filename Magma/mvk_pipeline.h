#pragma once

#include "pipeline.h"
#include <vulkan\vulkan.hpp>

class MVkContext;

class MVkPipeline : public MPipeline {
public:
  MVkPipeline(MVkContext* context, MVkPipelineParams& params) : context(context) { init(params); }

  void render() override;

private:
  MVkContext*      context;
  MVKPipeline      pipeline;

  uint32_t         vertexCount;

  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkBuffer>                vertexBuffers;
  std::vector<VkDescriptorSet>         descriptorSets;
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkSubpassDescription>    subpasses;
  std::vector<VkImageView>             colorAttachments;
  VkImageView                          depthAttachment;
  VkDescriptorSetLayout                descriptorSetLayout;

  void init(const MVkPipelineParams& params) override;
  void initPipelineState();
  void initRenderPass();
  void initFramebuffers();
  void initStages();
  void initPipelineLayout();
  void initPipelineCache();
  void initPipeline();
  void registerCommandBuffer();
};
