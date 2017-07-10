#pragma once
#include <vulkan\vulkan.hpp>
#include "pipeline.h"
#include "mvk_structs.h"


class MVkContext;
struct Camera;
struct Fluid;

class MVkPipeline : public MPipeline {
  friend class MVkContext;
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

  VkBuffer       vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  void*          vertexBufferMappedMemory;

  std::vector<VkCommandBuffer>         drawCmds;
  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkDescriptorSet>         descriptorSets;
  std::vector<VkAttachmentDescription> attachments;
  std::vector<VkSubpassDescription>    subpasses;
  std::vector<VkSubpassDependency>     dependencies;
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
  void updateDescriptorSets();
  void initCommandBuffers();
  void updateBuffers();
};
