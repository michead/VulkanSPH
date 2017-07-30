#pragma once
#include "gfx_context.h"
#include "fluid_simulation.h"

struct MagmaContext {
  GfxContext* graphics;
  FluidSimulation* fluidSimulation;
};
