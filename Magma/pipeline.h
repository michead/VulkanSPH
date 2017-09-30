#pragma once

#include <vector>
#include "gfx_structs.h"

struct MagmaContext;
class Subpass;
struct Scene;
struct SceneElement;

/**
 * Graphics pipeline abstraction
 */
class Pipeline {
public:
  Pipeline(const MagmaContext* context, Scene* scene, SceneElement* elem);

  virtual void init();
  virtual void postInit();
  virtual void update() {}
  virtual void draw() {}

  VkRenderPass getRenderPass() const;

protected:
  virtual void initPipelineState() {}
  virtual void initRenderPass() {}
  virtual void initVertexBuffer() {}
  virtual void updateBuffers() {}

  const MagmaContext* context;

  Scene*         scene;
  SceneElement*  elem;

  VkBuffer       vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  void*          vertexBufferMappedMemory;

  VkRenderPass renderPass;

  std::vector<VkImageView>             colorAttachments;
  MVkBufferDesc                        vertexBufferDesc;
  VkImageView                          depthAttachment;
  std::vector<VkAttachmentDescription> attachments = MVkBaseAttachments;
  std::vector<Subpass*>                subpasses;
};

extern const MVkPipelineParams BasePipeline;