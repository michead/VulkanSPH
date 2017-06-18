#pragma once

#include <vector>
#include "mvk_structs.h"

/**
 * Graphics pipeline abstraction
 */
class MPipeline {
public:
  MPipeline() { }

  virtual void init(const MVkPipelineParams& params) = 0;
  virtual void render() = 0;
};

extern const MVkPipelineParams BasePipeline;