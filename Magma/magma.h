#pragma once

#include <SDL2\SDL_syswm.h>
#include "config.h"
#include "scene.h"
#include "emitter.h"
#include "sph.h"

// Magma display name
#define MAGMA_DISPLAY_NAME "Magma"

// Magma version number
#define MAGMA_VERSION 1

// Success code
#define MAGMA_SUCCESS 0

// Fatal error code
#define MAGMA_FATAL   1

// INI Config file
#define MAGMA_CONFIG_FILENAME "magma.ini"

// Error code type
#define MAGMA_RESULT int32_t

// Forward declarations
class VkContext;

class Magma {
public:
  Magma() { init(); }
  ~Magma() { cleanup(); }

  // Listen for user input
  void mainLoop();

  // Engine configuration
  Config config;

  // Graphics context;
  VkContext* vkContext;

  // SDL window handle
  SDL_Window* window;

  // Frame delta time
  double deltaTime;

  // Scene graph and camera
  Scene scene;

  // Particle emitter
  Emitter* emitter;

  // SPH solver
  SPH* sph;

private:
  void init();
  void update(double deltaTime);
  void render(double deltaTime);
  void cleanup();
};