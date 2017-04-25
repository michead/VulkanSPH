/*
 * Vulkan Windowed Program
 *
 * Copyright (C) 2016 Valve Corporation
 * Copyright (C) 2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
Vulkan Windowed Project Template
Create and destroy a Vulkan surface on an SDL window.
*/

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
#include "vk_wrap.h"
#include "vk_log.h"

int createVulkanSurface(VkInstance instance, SDL_Window* window, VkSurfaceKHR* surface);

int main()
{
  // Create the Vulkan instance.
  VkInstance instance;
  VkResult result = VkWrap::createInstance(instance);
  if(result == VK_ERROR_INCOMPATIBLE_DRIVER) {
    logger->error("Unable to find a compatible Vulkan Driver.");
    return MAGMA_FATAL;
  } else if (result) {
    logger->error("Could not create a Vulkan instance (for unknown reasons).");
    return MAGMA_FATAL;
  }

  // Create an SDL window that supports Vulkan and OpenGL rendering.
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "Could not initialize SDL." << std::endl;
    return 1;
  }
  SDL_Window* window = SDL_CreateWindow(MAGMA_DISPLAY_NAME, SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
  if(window == NULL) {
    std::cout << "Could not create SDL window." << std::endl;
    return 1;
  }

  // Create a Vulkan surface for rendering
  VkSurfaceKHR surface;
  createVulkanSurface(instance, window, &surface);

  // This is where most initialization for a program should be performed

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
  vkDestroySurfaceKHR(instance, surface, NULL);
  SDL_DestroyWindow(window);
  SDL_Quit();
  vkDestroyInstance(instance, NULL);

  return 0;
}


int createVulkanSurface(VkInstance instance, SDL_Window* window, VkSurfaceKHR* surface)
{
  SDL_SysWMinfo windowInfo;
  SDL_VERSION(&windowInfo.version);
  if(!SDL_GetWindowWMInfo(window, &windowInfo)) {
    std::cout << "SDK window manager info is not available." << std::endl;
    return 1;
  }

  switch(windowInfo.subsystem) {

#if defined(SDL_VIDEO_DRIVER_ANDROID) && defined(VK_USE_PLATFORM_ANDROID_KHR)
  case SDL_SYSWM_ANDROID: {
    VkAndroidSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.window = windowInfo.info.android.window;

    VkResult result = vkCreateAndroidSurfaceKHR(instance, &surfaceInfo, NULL, surface);
    if(result != VK_SUCCESS) {
      std::cout << "Failed to create Android surface." << std::endl;
      return 1;
    }
    break;
  }
#endif

#if defined(SDL_VIDEO_DRIVER_MIR) && defined(VK_USE_PLATFORM_MIR_KHR)
  case SDL_SYSWM_MIR: {
    VkMirSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_MIR_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.connection = windowInfo.info.mir.connection;
    surfaceInfo.mirSurface = windowInfo.info.mir.surface;

    VkResult result = vkCreateMirSurfaceKHR(instance, &surfaceInfo, NULL, surface);
    if(result != VK_SUCCESS) {
      std::cout << "Failed to create Mir surface." << std::endl;
      return 1;
    }
    break;
  }
#endif

#if defined(SDL_VIDEO_DRIVER_WAYLAND) && defined(VK_USE_PLATFORM_WAYLAND_KHR)
  case SDL_SYSWM_WAYLAND: {
    VkWaylandSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.display = windowInfo.info.wl.display;
    surfaceInfo.surface = windowInfo.info.wl.surface;

    VkResult result = vkCreateWaylandSurfaceKHR(instance, &surfaceInfo, NULL, surface);
    if(result != VK_SUCCESS) {
      std::cout << "Failed to create Wayland surface." << std::endl;
      return 1;
    }
    break;
  }
#endif

#if defined(SDL_VIDEO_DRIVER_WINDOWS) && defined(VK_USE_PLATFORM_WIN32_KHR)
  case SDL_SYSWM_WINDOWS: {
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(NULL);
    surfaceInfo.hwnd = windowInfo.info.win.window;

    VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, NULL, surface);
    if(result != VK_SUCCESS) {
      std::cout << "Failed to create Win32 surface." << std::endl;
      return 1;
    }
    break;
  }
#endif

#if defined(SDL_VIDEO_DRIVER_X11) && defined(VK_USE_PLATFORM_XLIB_KHR)
  case SDL_SYSWM_X11: {
    VkXlibSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.dpy = windowInfo.info.x11.display;
    surfaceInfo.window = windowInfo.info.x11.window;

    VkResult result = vkCreateXlibSurfaceKHR(instance, &surfaceInfo, NULL, surface);
    if(result != VK_SUCCESS) {
      std::cout << "Failed to create X11 surface." << std::endl;
      return 1;
    }
    break;
  }
#endif

  default:
    std::cout << "Unsupported window manager is in use." << std::endl;
    return 1;
  }

  return 0;
}
