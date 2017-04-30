// Vulkan Win API
#define VK_USE_PLATFORM_WIN32_KHR

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#define SDL_CHECK(a)          ; // nop
#define SDL_CHECK_NOT_NULL(a) ; // nop

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "magma.h"
#include "vk_wrap.h"

void Magma::init() {
  // Load configs from INI file
  config = Config::load(MAGMA_CONFIG_FILENAME);

  // Create SDL window
  window = SDL_CreateWindow(
    MAGMA_DISPLAY_NAME,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    config.resolution.x,
    config.resolution.y, SDL_WINDOW_OPENGL);

  SDL_SysWMinfo windowInfo;
  SDL_VERSION(&windowInfo.version);
  SDL_CHECK_NOT_NULL(SDL_GetWindowWMInfo(window, &windowInfo));

  // Initialize graphics context
  vkContext = VkContext::getContext();
  VK_CHECK(vkContext->init(windowInfo.info.win.window));

  // Create an SDL window that supports Vulkan and OpenGL rendering.
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
  SDL_CHECK_NOT_NULL(window);
}

void Magma::cleanup() {
  // Clean up.
  vkDestroySurfaceKHR(vkContext->instance, vkContext->surface, NULL);
  SDL_DestroyWindow(window);
  SDL_Quit();
  vkDestroyInstance(vkContext->instance, NULL);
}

void Magma::mainLoop() {
  bool stillRunning = true;
  while (stillRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        stillRunning = false;
        break;
      default:
        break;
      }
    }
    SDL_Delay(10);
  }
}