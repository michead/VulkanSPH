#pragma once

#include "pipeline.h"
#include "mvk_structs.h"
#include <glm\glm.hpp>
#include <vulkan\vulkan.hpp>

class MVkContext;
struct Camera;
struct Fluid;

class MVkPipeline : public MPipeline {
public:
  MVkPipeline(MVkContext* context, Camera* camera, Fluid* fluid) : context(context), camera(camera), fluid(fluid) { init(); }

  virtual void update() override;

  VkCommandBuffer getDrawCmdBuffer() const;

private:
  MVkContext*      context;
  MVKPipeline      pipeline;

  Camera* camera;
  Fluid*  fluid;

  MVkVertexShaderUniformParticle   uniformsVS;
  MVkFragmentShaderUniformParticle uniformsFS;

  std::vector<VkCommandBuffer>         drawCmds;
  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkDescriptorSet>         descriptorSets;
  std::vector<VkBuffer>                vertexBuffers;
  std::vector<VkDeviceMemory>          vertexBufferMemoryVec;
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkSubpassDescription>    subpasses;
  std::vector<VkImageView>             colorAttachments;
  MVkUniformBuffer                     uniformBufferVS;
  MVkUniformBuffer                     uniformBufferFS;
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
  void initCommandBuffers();
};
