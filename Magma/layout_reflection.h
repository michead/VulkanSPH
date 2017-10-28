#pragma once
#include <vulkan\vulkan.hpp>
#include <spirv_glsl.hpp>
#include <vector>

typedef std::vector<uint32_t> SpirV;

class LayoutReflection {
public:
  LayoutReflection(SpirV spirv, VkShaderStageFlags stageFlags);

  void extractResourcesfromSpirV();
  void extractUniformBufferDescSetLayout();

private:
  VkDescriptorSetLayoutBinding    descSetLayoutBindingFromResource(spirv_cross::Resource resource, VkDescriptorType descType);
  VkDescriptorSetLayoutCreateInfo descSetLayoutFromBindings(std::vector<VkDescriptorSetLayoutBinding> bindings);

  spirv_cross::CompilerGLSL    glsl;
  VkShaderStageFlags           stageFlags;
  SpirV                        spirv;
  spirv_cross::ShaderResources resources;

  VkDescriptorSetLayoutCreateInfo           descSetLayout;
  std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings;
};
