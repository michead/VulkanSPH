#pragma once
#include <vulkan\vulkan.hpp>
#include <spirv_glsl.hpp>
#include <vector>

typedef std::vector<uint32_t> SpirV;

class LayoutReflection {
public:
  LayoutReflection(SpirV spirv);

  spirv_cross::ShaderResources fromSpirV(SpirV spirv);
  std::vector<VkDescriptorSetLayoutCreateInfo> extractUniformBufferDescSetLayouts();

private:
  VkDescriptorSetLayoutCreateInfo descSetLayoutFromUniformBuffer(spirv_cross::Resource resource);

  SpirV                        spirv;
  spirv_cross::ShaderResources resources;

  std::vector<VkDescriptorSetLayoutCreateInfo> descSetLayouts;
};
