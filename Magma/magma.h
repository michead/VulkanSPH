#pragma once

#include <iostream>
#include <SDL2\SDL_syswm.h>
#include "config.h"
#include "scene.h"
#include "emitter.h"
#include "sph.h"
#include "mvk_context.h"
#include "mvk_pipeline.h"

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
  
  Config config;
  MVkContext* mvkContext;
  MVkPipeline* mvkPipeline;
  SDL_Window* window;
  double deltaTime;
  Scene scene;
  Emitter* emitter;
  SPH* sph;

private:
  void init();
  void update(double deltaTime);
  void render(double deltaTime);
  void cleanup();
};