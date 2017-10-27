#pragma once
#include <vulkan\vulkan.hpp>
#include "pipeline.h"
#include "gfx_structs.h"

struct MagmaContext;
struct Scene;
struct Fluid;

class FluidPipeline : public Pipeline {
  friend struct MagmaContext;
public:
  FluidPipeline(const MagmaContext* context, Scene* scene, Fluid* elem);

  virtual void init()              override;
  virtual void registerSubpasses() override;
  virtual void postInit()          override;
  virtual void update()            override;

protected:
  virtual void initRenderPass()    override;
  virtual void initVertexBuffers() override;
  virtual void updateBuffers()     override;
};
