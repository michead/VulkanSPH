#define SDL_MAIN_HANDLED
#include <SDL2\SDL.h>
#include <SDL2\SDL_syswm.h>
#include "hud.h"
#include "imgui_sdl_vulkan_bindings.h"
#include "gfx_context.h"
#include "gfx_wrap.h"

HUD::HUD(const GfxContext* context, SDL_Window* window) : context(context) {
  ImGui_ImplSDLVulkan_Init_Data init_data = {};
  init_data.allocator       = nullptr;
  init_data.gpu             = context->physicalDevice;
  init_data.device          = context->device;
  init_data.render_pass     = context->getRenderPass();
  init_data.pipeline_cache  = context->getPipelineCache();
  init_data.descriptor_pool = context->descriptorPool;
  init_data.viewportSize = {
    (uint32_t)context->viewport.width,
    (uint32_t)context->viewport.height
  };
  ImGui_ImplSDLVulkan_Init(window, &init_data);

  VK_CHECK(vkResetCommandPool(context->device, context->getCurrentCmdPool(), 0));

  VkCommandBuffer drawCmd = context->getCurrentCmdBuffer();
  VkCommandBufferBeginInfo begin_info = {};
  
  begin_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  VK_CHECK(vkBeginCommandBuffer(context->getCurrentCmdBuffer(), &begin_info));

  ImGui_ImplSDLVulkan_CreateFontsTexture(drawCmd);

  VkSubmitInfo end_info       = {};
  end_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  end_info.commandBufferCount = 1;
  end_info.pCommandBuffers    = &drawCmd;
  VK_CHECK(vkEndCommandBuffer(drawCmd));
  VK_CHECK(vkQueueSubmit(context->graphicsQueue, 1, &end_info, VK_NULL_HANDLE));

  VK_CHECK(vkDeviceWaitIdle(context->device));
  ImGui_ImplSDLVulkan_InvalidateFontUploadObjects();
}

HUD::~HUD() {
  ImGui_ImplSDLVulkan_Shutdown();
}

void HUD::setupNewFrame() {
  ImGui_ImplSDLVulkan_NewFrame();
}

void HUD::render() {
  for each (auto drawFn in drawFns) {
    ImGui::Begin(drawFn.first);
    ImGui::BeginGroup();
    drawFn.second();
    ImGui::EndGroup();
    ImGui::End();
  }

  ImGui_ImplSDLVulkan_Render(context->getCurrentCmdBuffer());
}

void HUD::registerWindow(const char* label, std::function<void()> drawFn) {
  drawFns.push_back(std::make_pair(label, drawFn));
}

void HUD::unregisterWindow(const char* label) {
  for (auto it = drawFns.begin(); it != drawFns.end(); it++) {
    if (strcmp((*it).first, label) == 0) {
      drawFns.erase(it);
    }
  }
}

void HUD::group(const char* label, std::function<void()> drawInnerFn, bool isCollapsed, bool isIndented) {
  if (isIndented) {
    ImGui::Indent();
  }

  ImGui::PushID(label);

  static bool isFirstRun = true;
  if (isFirstRun && !isCollapsed) {
    isFirstRun = false;
    ImGui::SetNextTreeNodeOpen(true);
  }
  
  if (ImGui::CollapsingHeader(label)) {
    drawInnerFn();
  }
  
  ImGui::PopID();

  if (isIndented) {
    ImGui::Unindent();
  }
}

void HUD::vec3Slider(const char* label, float* val, float minVal, float maxVal, const char* format) {
  ImGui::SliderFloat3(label, val, minVal, maxVal, format);
}
