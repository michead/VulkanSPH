#pragma once
#include "subpass.h"

class FluidSubpass0 : public Subpass {
public:
  FluidSubpass0(const Pipeline* pipeline, uint8_t index, SceneElement* elem) : Subpass(pipeline, "fluid_0", index, elem) { }

  virtual void init()                 override;
  virtual void postInit()             override;
  virtual void initUniformBuffers()   override;
  virtual void initGraphicsPipeline() override;
  virtual void initVertexBuffers()    override;
  virtual void update()               override;
  virtual void updateDescriptorSets() override;
  virtual void updateUniformBuffers() override;

private:
  MVkVert0 uniformsVS;
  MVkFrag0 uniformsFS;
};
