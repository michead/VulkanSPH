#pragma once
#include <vulkan\vulkan.hpp>
#include "pipeline.h"
#include "gfx_structs.h"

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
  virtual void initRenderPass()       override;
  virtual void initFramebuffers()     override;
  virtual void initVertexBuffer()     override;
  virtual void initCommandBuffers()   override;
  virtual void updateBuffers()        override;
};
