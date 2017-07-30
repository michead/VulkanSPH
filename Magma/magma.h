#pragma once
#include <SDL2\SDL_syswm.h>
#include <iostream>
#include "magma_context.h"
#include "config.h"
#include "event_handler.h"
#include "fluid_simulation.h"
#include "perf.h"
#include "scene.h"
#include "gfx_context.h"
#include "pipeline.h"

// Magma display name
#define MAGMA_DISPLAY_NAME "Magma"
#define MAGMA_VERSION 1
#define MAGMA_SUCCESS 0
#define MAGMA_FATAL   1
#define MAGMA_CONFIG_FILENAME "magma.ini"
#define MAGMA_RESULT int32_t

class Magma {
public:
  Magma() { init(); }
  ~Magma() { cleanup(); }

  void mainLoop();
  
  MagmaContext     context;
  Config           config;
  GfxContext*      mvkContext;
  Pipeline*        pipeline;
  Scene*           scene;
  SDL_Window*      window;
  EventHandler*    eventHandler;
  FluidSimulation* fluidSimulation;
  Perf*            perf;
  bool             shouldQuit = false;

private:
  void init();
  void update(double deltaTime);
  void render(double deltaTime);
  void cleanup();
};
