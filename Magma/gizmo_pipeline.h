#pragma once
#include <vulkan\vulkan.hpp>
#include "pipeline.h"

struct MagmaContext;

class GizmoPipeline : public Pipeline {
public:
  GizmoPipeline(const MagmaContext* context, Scene* scene);
};
