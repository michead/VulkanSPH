#pragma once

#include <vector>
#include "gfx_structs.h"

class GfxContext;
class Subpass;
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
  virtual void update() {}

  VkCommandBuffer getDrawCmdBuffer() const;
  VkRenderPass    getRenderPass()    const;

protected:
  virtual void initPipelineState() {}
  virtual void initRenderPass() {}
  virtual void initFramebuffers() {}
  virtual void initVertexBuffer() {}
  virtual void initCommandBuffers() {}
  virtual void updateBuffers() {}

  GfxContext* context;

  Scene*         scene;
  SceneElement*  elem;

  VkBuffer       vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  void*          vertexBufferMappedMemory;

  VkRenderPass renderPass;

  std::vector<VkCommandBuffer>         drawCmds;
  std::vector<VkFramebuffer>           framebuffers;
  std::vector<VkImageView>             colorAttachments;
  MVkBufferDesc                        vertexBufferDesc;
  VkImageView                          depthAttachment;
  std::vector<VkAttachmentDescription> attachments = MVkBaseAttachments;
  std::vector<Subpass*>                subpasses;
};

extern const MVkPipelineParams BasePipeline;