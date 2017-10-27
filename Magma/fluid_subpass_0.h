#pragma once
#include "subpass.h"

class FluidSubpass0 : public Subpass {
public:
  FluidSubpass0(const MagmaContext* context, VkRenderPass* renderPass, SceneElement* elem, uint8_t index)
    : Subpass(context, renderPass, elem, "fluid_0", index) { }

  virtual void init()                 override;
  virtual void postInit()             override;
  virtual void initUniformBuffers()   override;
  virtual void update()               override;
  virtual void updateUniformBuffers() override;

private:
  MVkVert0 uniformsVS;
  MVkFrag0 uniformsFS;
};
