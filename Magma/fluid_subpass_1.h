#pragma once
#include "subpass.h"

class FluidSubpass1 : public Subpass {
public:
  FluidSubpass1(const Pipeline* pipeline, uint8_t index, SceneElement* elem) : Subpass(pipeline, "fluid_1", index, elem) { }

  virtual void init()                 override;
  virtual void postInit()             override;
  virtual void initUniformBuffers()   override;
  virtual void update()               override;
  virtual void updateDescriptorSets() override;
  virtual void updateUniformBuffers() override;

private:
  MVkFrag1 uniformsFS;
};
