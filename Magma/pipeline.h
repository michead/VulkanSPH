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
  virtual void registerSubpasses() {}
  virtual void postInit();
  virtual void update() {}
  virtual void draw();

  const MagmaContext*      getContext() const;
  VkRenderPass             getRenderPass() const;
  std::vector<VkImageView> getColorAttachments() const;
  VkImageView              getDepthAttachment() const;
  std::vector<Subpass*>    getSubpasses() const;

protected:
  virtual void initPipelineState() {}
  virtual void initRenderPass() {}
  virtual void initVertexBuffers();
  virtual void updateBuffers() {}

  const MagmaContext* context;

  Scene*         scene;
  SceneElement*  elem;

  VkRenderPass renderPass;

  std::vector<VkImageView>             colorAttachments;
  MVkBufferDesc                        vertexBufferDesc;
  MVkBufferDesc                        fsQuadBufferDesc;
  VkImageView                          depthAttachment;
  std::vector<VkAttachmentDescription> attachments = MVkBaseAttachments;
  std::vector<Subpass*>                subpasses;
};

extern const MVkPipelineParams BasePipeline;