#pragma once

// Readapted from ImGui Vulkan example https://github.com/ocornut/imgui/tree/master/examples/vulkan_example

#include "gfx_wrap.h"

struct SDL_Window;

#define IMGUI_VK_QUEUED_FRAMES 2

struct ImGui_ImplSDLVulkan_Init_Data {
  VkAllocationCallbacks* allocator;
  VkPhysicalDevice       gpu;
  VkDevice               device;
  VkRenderPass           render_pass;
  VkPipelineCache        pipeline_cache;
  VkDescriptorPool       descriptor_pool;
  VkExtent2D             viewportSize;
  void(*check_vk_result)(VkResult err);
};

void ImGui_ImplSDLVulkan_Init(SDL_Window* window, ImGui_ImplSDLVulkan_Init_Data *init_data);
void ImGui_ImplSDLVulkan_Shutdown();
void ImGui_ImplSDLVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
void ImGui_ImplSDLVulkan_InvalidateFontUploadObjects();
void ImGui_ImplSDLVulkan_NewFrame();
void ImGui_ImplSDLVulkan_Render(VkCommandBuffer command_buffer);
