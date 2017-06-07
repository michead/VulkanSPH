#pragma once

#include <vulkan\vulkan.hpp>

// Forward declaration
class VkContext;

class VkPath {
public:
  VkPath(VkContext* context) : context(context) { init(); }
  virtual ~VkPath() { }

private:
  void init() { initCommandBuffer(); }
  void initCommandBuffer();

  VkContext* context;
  VkCommandBuffer commandBuffer;
  VkPipeline pipeline;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkFramebuffer framebuffer;
  std::vector<VkDescriptorSet> descriptorSets;
  std::vector<VkBuffer> vertexBuffers;
  uint32_t vertexCount;
};