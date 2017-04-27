// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include "vk_context.h"
#include "vk_log.h"

#define SDL_CHECK(a) VK_CHECK(a)
#define SDL_CHECK_NOT_NULL(a) VK_CHECK(a != NULL)

int main() {
  SDL_Window* window = SDL_CreateWindow(
    MAGMA_DISPLAY_NAME,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);

  SDL_SysWMinfo windowInfo;
  SDL_VERSION(&windowInfo.version);
  SDL_CHECK_NOT_NULL(SDL_GetWindowWMInfo(window, &windowInfo));

  // Initialize graphics context
  VkContext& vkContext = VkContext::getContext();
  VK_CHECK(vkContext.init(windowInfo.info.win.window));

  // Create an SDL window that supports Vulkan and OpenGL rendering.
  SDL_CHECK(SDL_Init(SDL_INIT_VIDEO));
  SDL_CHECK_NOT_NULL(window);

  // Poll for user input.
  bool stillRunning = true;
  while(stillRunning) {

  SDL_Event event;
  while(SDL_PollEvent(&event)) {

    switch(event.type) {

    case SDL_QUIT:
      stillRunning = false;
      break;

    default:
      // Do nothing.
      break;
    }
  }

    SDL_Delay(10);
  }

  // Clean up.
  vkDestroySurfaceKHR(vkContext.instance, vkContext.surface, NULL);
  SDL_DestroyWindow(window);
  SDL_Quit();
  vkDestroyInstance(vkContext.instance, NULL);

  return MAGMA_SUCCESS;
}
