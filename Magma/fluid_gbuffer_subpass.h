#pragma once
#include "subpass.h"

class FluidGBufferSubpass : public Subpass {
public:
  FluidGBufferSubpass(GfxContext* context, VkRenderPass* renderPass, SceneElement* elem, uint8_t index) : Subpass(context, renderPass, elem, index) {}

  virtual void init()     override;
  virtual void postInit() override;
  virtual void update()   override;

private:
  MVkVertexShaderUniformParticle   uniformsVS;
  MVkFragmentShaderUniformParticle uniformsFS;
};
