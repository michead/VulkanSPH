// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#define SDL_CHECK(a)          ; // nop
#define SDL_CHECK_NOT_NULL(a) ; // nop
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <NvFlex.h>
#include "mvk_wrap.h"
#include "magma.h"

// Initialize Flex library
NvFlexLibrary* flexLibrary = NvFlexInit();

void Magma::init() {
  // Load configs from INI file
  config = Config::load(MAGMA_CONFIG_FILENAME);

  // Initialize event handler and perf modules
  eventHandler = new EventHandler();
  perf         = new Perf();

  // Register callback for engine exit
  eventHandler->addListener(EVT_QUIT, [this](Event evt) { shouldQuit = true; });

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
  while (!shouldQuit) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
      // Update timers
      perf->onFrameStart();
      // Notify event listeners
      eventHandler->handle(event);
    }
    
    // Update simulation
    update(perf->getLastDeltaTime());
    // Render scene
    render(perf->getLastDeltaTime());

    SDL_Delay(10);
  }
}