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

  VkFormat         format;
  VkRenderPass     renderPass;
  VkPipelineLayout pipelineLayout;
  uint32_t         vertexCount;

  MVKPipeline      pipeline;

  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkBuffer>                vertexBuffers;
  std::vector<VkDescriptorSet>         descriptorSets;
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkSubpassDescription>    subpasses;
  std::vector<VkImageView>             colorAttachments;
  VkImageView                          depthAttachment;

  void init(const MVkPipelineParams& params) override;
  void initSubpasses();
  void initRenderPass();
  void initFramebuffers();
  void initStages();
  void initPipeline();
  void registerCommandBuffer();
};
