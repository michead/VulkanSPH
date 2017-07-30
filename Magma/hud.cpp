#define SDL_MAIN_HANDLED
#include <SDL2\SDL.h>
#include <SDL2\SDL_syswm.h>
#include "hud.h"
#include "imgui_sdl_vulkan_bindings.h"
#include "gfx_context.h"

HUD::HUD(const GfxContext* context, SDL_Window* window) : context(context) {
  ImGui_ImplSDLVulkan_Init_Data init_data = {};
  init_data.allocator       = nullptr;
  init_data.gpu             = context->physicalDevice;
  init_data.device          = context->device;
  init_data.render_pass     = context->getRenderPass();
  init_data.pipeline_cache  = context->getPipelineCache();
  init_data.descriptor_pool = context->descriptorPool;
  ImGui_ImplSDLVulkan_Init(window, &init_data);
}

void HUD::render() {
  // TODO: Init ImGui's VkCommandBuffer
  ImGui_ImplSDLVulkan_Render(VK_NULL_HANDLE);
}
