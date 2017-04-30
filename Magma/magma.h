#pragma once

#include <SDL2\SDL_syswm.h>
#include "config.h"
#include "vk_context.h"

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

private:
  void init();
  void cleanup();
};