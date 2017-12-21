// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#define SDL_CHECK(a)          ; // nop
#define SDL_CHECK_NOT_NULL(a) ; // nop
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "gfx_wrap.h"
#include "magma.h"

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
  mvkContext = GfxContext::getContext(
    MAGMA_DISPLAY_NAME,
    MAGMA_VERSION, 
    windowInfo.info.win.window,
    &config);

  // Init fluid simulation
  fluidSimulation = new FluidSimulation();

  // Set context object
  context.graphics        = mvkContext;
  context.fluidSimulation = fluidSimulation;
  context.hud             = hud;

  // Load scene attributes and bind graphics context
  scene = new Scene("data/scene.json", &context);

  // Initialize ImGui
  hud = new HUD(&context, window);
  context.hud = hud;

  // Set scene in physics and graphics modules
  context.fluidSimulation->initCollision(scene);
  context.graphics->setScene(scene);
  context.graphics->postInit();
  context.graphics->getPipeline()->postInit();

  // Build HUD for debugging
  registerHUDExtensions();

  // Register input callbacks for camera movement
  registerCameraMovement();
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
  fluidSimulation->update();
}

void Magma::registerHUDExtensions() {
  hud->registerWindow("Scene Explorer", [this]() {
    hud->group("Camera", [this]() {
      hud->vec3Slider("Position", &scene->camera->pos.x);
    }, false);
    hud->group("Ligths", [this]() {
      for each(auto& light in scene->lights) {
        hud->vec3Slider("Position", &light->pos.x);
      }
    }, false);
  });
}

void Magma::registerCameraMovement() {
  eventHandler->addListener(EVT_MOUSE_DOWN_LEFT_BTN,  [this](Event evt) {
    // No binding yet
  });
  eventHandler->addListener(EVT_MOUSE_DOWN_RIGHT_BTN, [this](Event evt) {
    // No binding yet
  });
  eventHandler->addListener(EVT_MOUSE_DRAG_LEFT_BTN,  [this](Event evt) {
    if (evt.key.keysym.mod & KMOD_ALT) {
      scene->camera->orbit(glm::vec3(0), (float)evt.motion.xrel, (float)evt.motion.yrel);
    }
  });
  eventHandler->addListener(EVT_MOUSE_DRAG_RIGHT_BTN, [this](Event evt) {
    scene->camera->rotate((float)evt.motion.xrel, (float)evt.motion.yrel);
  });
  eventHandler->addListener(EVT_MOUSE_WHEEL_SCROLL,   [this](Event evt) {
    scene->camera->dolly((float)evt.wheel.y);
  });
}

void Magma::prepareFrame() {
  hud->setupNewFrame();

  bool shouldResize;
  GfxWrap::prepareFrame(
    mvkContext->device,
    mvkContext->swapchain.handle,
    mvkContext->imageAcquiredSemaphore,
    &mvkContext->currentImageIndex,
    &shouldResize);

  if (shouldResize) {
    eventHandler->handle(EVT_VIEWPORT_CHANGED);
    return;
  }

  VK_CHECK(vkResetFences(
    mvkContext->device,
    1,
    &mvkContext->drawFences[mvkContext->currentImageIndex]));

  VkCommandBuffer drawCmd = mvkContext->getCurrentCmdBuffer();
  std::array<VkClearValue, 2> clearValues = GfxWrap::clearValues();

  vkResetCommandPool(
    mvkContext->device,
    mvkContext->getCurrentCmdPool(),
    0);
  GfxWrap::beginCommandBuffer(
    mvkContext->getCurrentCmdBuffer(),
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  GfxWrap::beginRenderPass(
    drawCmd,
    mvkContext->getRenderPass(),
    mvkContext->getCurrentFramebuffer(),
    mvkContext->swapchain.extent,
    clearValues.data());

  vkCmdSetViewport(drawCmd, 0, 1, &mvkContext->viewport);
  vkCmdSetScissor(drawCmd, 0, 1, &mvkContext->scissor);
}

void Magma::presentFrame() {
  VkCommandBuffer drawCmd = mvkContext->getCurrentCmdBuffer();
  VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  vkCmdEndRenderPass(drawCmd);
  VK_CHECK(vkEndCommandBuffer(drawCmd));

  GfxWrap::submitCommandBuffer(
    mvkContext->graphicsQueue,
    1,
    &drawCmd,
    1,
    &mvkContext->imageAcquiredSemaphore,
    &stageFlags,
    mvkContext->drawFences[mvkContext->currentImageIndex]);

  VkResult res;
  do {
    VK_CHECK((res = vkWaitForFences(
      mvkContext->device,
      1,
      &mvkContext->drawFences[mvkContext->currentImageIndex],
      VK_TRUE,
      UINT64_MAX)));
  } while (res == VK_TIMEOUT);

  GfxWrap::presentSwapchain(
    mvkContext->presentQueue,
    &mvkContext->swapchain.handle,
    &mvkContext->currentImageIndex);

  vkQueueWaitIdle(mvkContext->presentQueue);
}

void Magma::render(double deltaTime) {
  prepareFrame();

  scene->render();
  hud->render();

  presentFrame();
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

    vkDeviceWaitIdle(mvkContext->device);
  }
}