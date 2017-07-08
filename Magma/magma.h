#pragma once
#include <SDL2\SDL_syswm.h>
#include <iostream>
#include "config.h"
#include "event_handler.h"
#include "perf.h"
#include "scene.h"
#include "mvk_context.h"
#include "mvk_pipeline.h"

// Magma display name
#define MAGMA_DISPLAY_NAME "Magma"
#define MAGMA_VERSION 1
#define MAGMA_SUCCESS 0
#define MAGMA_FATAL   1
#define MAGMA_CONFIG_FILENAME "magma.ini"
#define MAGMA_RESULT int32_t

struct NvFlexLibrary;

class Magma {
public:
  Magma() { init(); }
  ~Magma() { cleanup(); }

  void mainLoop();
  
  Config        config;
  MVkContext*   mvkContext;
  MVkPipeline*  mvkPipeline;
  Scene*        scene;
  SDL_Window*   window;
  EventHandler* eventHandler;
  Perf*         perf;
  bool          shouldQuit = false;

private:
  void init();
  void update(double deltaTime);
  void render(double deltaTime);
  void cleanup();
};

extern NvFlexLibrary* flexLibrary;