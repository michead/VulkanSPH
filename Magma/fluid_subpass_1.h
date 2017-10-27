#pragma once
#include "subpass.h"

class FluidSubpass1 : public Subpass {
public:
  FluidSubpass1(const MagmaContext* context, VkRenderPass* renderPass, SceneElement* elem, uint8_t index)
    : Subpass(context, renderPass, elem, "fluid_1", index) { }

  virtual void init()                 override;
  virtual void postInit()             override;
  virtual void initUniformBuffers()   override;
  virtual void update()               override;
  virtual void updateUniformBuffers() override;

private:
  MVkFrag1        uniformsFS;
  VkCommandBuffer copyStagingVSBufferCmd;
  VkCommandBuffer copyStagingFSBufferCmd;
};
