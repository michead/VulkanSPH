#pragma once
#include "subpass.h"

class FluidSubpass2: public Subpass {
public:
  FluidSubpass2(const Pipeline* pipeline, uint8_t index, SceneElement* elem) : Subpass(pipeline, "fluid_2", index, elem) {}

  virtual void init()                 override;
  virtual void postInit()             override;
  virtual void initUniformBuffers()   override;
  virtual void initGraphicsPipeline() override;
  virtual void update()               override;
  virtual void updateDescriptorSets() override;
  virtual void updateUniformBuffers() override;

private:
  MVkFrag2 uniformsFS;
};
