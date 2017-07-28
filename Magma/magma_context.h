#pragma once
#include "mvk_context.h"
#include "fluid_simulation.h"

struct MagmaContext {
  MVkContext* graphics;
  FluidSimulation* fluidSimulation;
};
