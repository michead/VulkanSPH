#pragma once

#include <vector>
#include "mvk_structs.h"

/**
 * Graphics pipeline abstraction
 */
class MPipeline {
public:
  MPipeline() { }

  virtual void init() = 0;
  virtual void update() {}
};

extern const MVkPipelineParams BasePipeline;