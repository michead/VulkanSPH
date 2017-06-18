// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#define SDL_CHECK(a)          ; // nop
#define SDL_CHECK_NOT_NULL(a) ; // nop

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "magma.h"

void Magma::init() {
  // Load configs from INI file
  config = Config::load(MAGMA_CONFIG_FILENAME);

  // Create an SDL window that supports Vulkan and OpenGL rendering.
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow(
    MAGMA_DISPLAY_NAME,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    config.resolution.x,
    config.resolution.y, SDL_WINDOW_OPENGL);

  SDL_SysWMinfo windowInfo;
  SDL_VERSION(&windowInfo.version);
  SDL_GetWindowWMInfo(window, &windowInfo);

  // Initialize graphics context
  mvkContext = MVkContext::getContext(
    MAGMA_DISPLAY_NAME,
    MAGMA_VERSION, 
    windowInfo.info.win.window);

  // Initialize solver and emitter
  sph = new SPH("params.json");
  emitter = new Emitter();
  emitter->setSolver(sph);

  mvkPipeline = new MVkPipeline(mvkContext, MVkPipelineParams());
}

void Magma::cleanup() {
  // Clean up.
  vkDestroySurfaceKHR(mvkContext->instance, mvkContext->surface, NULL);
  SDL_DestroyWindow(window);
  SDL_Quit();
  vkDestroyInstance(mvkContext->instance, NULL);
  delete sph;
  delete emitter;
}

void Magma::update(double deltaTime) {
  sph->update(deltaTime);
}

void Magma::render(double deltaTime) {
  mvkPipeline->render();
}

void Magma::mainLoop() {
  bool stillRunning = true;
  uint64_t timeNow = SDL_GetPerformanceCounter();
  uint64_t timeOld = 0;
  
  while (stillRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      timeOld = timeNow;
      timeNow = SDL_GetPerformanceCounter();
      deltaTime = (double)((timeNow - timeOld) * 1000 / SDL_GetPerformanceFrequency());
      switch (event.type) {
      case SDL_QUIT:
        stillRunning = false;
        break;
      default:
        break;
      }
    }
    
    // Update simulation
    update(deltaTime);
    // Render scene
    render(deltaTime);

    SDL_Delay(10);
  }
}