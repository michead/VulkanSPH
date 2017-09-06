#pragma once
#include <vulkan\vulkan.hpp>
#include "pipeline.h"
#include "gfx_structs.h"

#define to_fluid(elem) dynamic_cast<Fluid*>(elem)

class GfxContext;
struct Scene;
struct Fluid;

class FluidPipeline : public Pipeline {
  friend class GfxContext;
public:
  FluidPipeline(GfxContext* context, Scene* scene, Fluid* elem);

  virtual void init()     override;
  virtual void postInit() override;
  virtual void update()   override;

protected:
  virtual void initPipelineState()    override;
  virtual void initRenderPass()       override;
  virtual void initFramebuffers()     override;
  virtual void initStages()           override;
  virtual void initPipelineLayout()   override;
  virtual void initPipelineCache()    override;
  virtual void initPipeline()         override;
  virtual void initVertexBuffer()     override;
  virtual void initUniformBuffers()   override;
  virtual void updateDescriptorSets() override;
  virtual void initCommandBuffers()   override;
  virtual void updateBuffers()        override;
};
