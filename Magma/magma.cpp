// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#define SDL_CHECK(a)          ; // nop
#define SDL_CHECK_NOT_NULL(a) ; // nop

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <NvFlex.h>
#include "mvk_wrap.h"
#include "magma.h"

// Initialize Flex library
NvFlexLibrary* flexLibrary = NvFlexInit();

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
    windowInfo.info.win.window,
    &config);

  // Load scene attributes and bind graphics context
  scene = new Scene("scene.json", mvkContext);
}

void Magma::cleanup() {
  vkDestroySurfaceKHR(mvkContext->instance, mvkContext->surface, NULL);
  SDL_DestroyWindow(window);
  SDL_Quit();
  vkDestroyInstance(mvkContext->instance, NULL);
  delete scene;
}

void Magma::update(double deltaTime) {
  scene->update();
}

void Magma::render(double deltaTime) {
  MVkWrap::prepareFrame(
    mvkContext->device,
    mvkContext->swapchain.handle,
    mvkContext->imageAcquiredSemaphore,
    &mvkContext->currentSwapchainImageIndex);

  VK_CHECK(vkResetFences(
    mvkContext->device,
    1,
    &mvkContext->drawFences[mvkContext->currentSwapchainImageIndex]));

  scene->render();

  VkResult res;
  do {
    VK_CHECK((res = vkWaitForFences(
      mvkContext->device,
      1,
      &mvkContext->drawFences[mvkContext->currentSwapchainImageIndex],
      VK_TRUE,
      UINT64_MAX)));
  } while (res == VK_TIMEOUT);

  MVkWrap::presentSwapchain(
    mvkContext->presentQueue,
    &mvkContext->swapchain.handle,
    &mvkContext->currentSwapchainImageIndex);
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