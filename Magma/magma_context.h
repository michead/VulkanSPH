#pragma once
#include "gfx_context.h"
#include "fluid_simulation.h"
#include "hud.h"

struct MagmaContext {
  GfxContext*      graphics;
  FluidSimulation* fluidSimulation;
  HUD*             hud;
};
