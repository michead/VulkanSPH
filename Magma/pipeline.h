#pragma once

#include <vector>
#include "gfx_structs.h"

class GfxContext;
struct Scene;
struct SceneElement;

/**
 * Graphics pipeline abstraction
 */
class Pipeline {
public:
  Pipeline(GfxContext* context, Scene* scene, SceneElement* elem);

  virtual void init();
  virtual void postInit();
  virtual void update();

  virtual VkCommandBuffer getDrawCmdBuffer() const;

  MVKPipeline pipeline;

protected:
  virtual void initPipelineState() {}
  virtual void initRenderPass() {}
  virtual void initFramebuffers() {}
  virtual void initStages() {}
  virtual void initPipelineLayout() {}
  virtual void initPipelineCache() {}
  virtual void initPipeline() {}
  virtual void initVertexBuffer() {}
  virtual void initUniformBuffers() {}
  virtual void updateDescriptorSets() {}
  virtual void initCommandBuffers() {}
  virtual void updateBuffers() {}

  GfxContext* context;

  Scene*         scene;
  SceneElement*  elem;

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
  MVkBufferDesc                        vertexBufferDesc;
  MVkBufferDesc                        uniformBufferVSDesc;
  MVkBufferDesc                        uniformBufferFSDesc;
  VkImageView                          depthAttachment;
};

extern const MVkPipelineParams BasePipeline;