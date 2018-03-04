#pragma once
#include <vulkan\vulkan.hpp>
#include <spirv_glsl.hpp>
#include <vector>

typedef std::vector<uint32_t> SpirV;

class LayoutReflection {
public:
  LayoutReflection(VkDevice device, SpirV spirv, VkShaderStageFlags stageFlags);

  void extractResourcesfromSpirV();
  void extractUniformBufferDescSetLayout();

  VkDescriptorSetLayout                          getDescriptorSetLayout()              const { return descSetLayout; }
  std::vector<VkDescriptorSetLayoutBinding>      getDescriptorSetLayoutBindings()      const { return descSetLayoutBindings; }
  std::vector<VkVertexInputBindingDescription>   getVertexInputBindingDescriptions()   const { return vertInputBindingDescs; }
  std::vector<VkVertexInputAttributeDescription> getVertexInputAttributeDescriptions() const { return vertInputAttrDescs; }

private:
  VkDescriptorSetLayoutBinding      descSetLayoutBindingFromResource(spirv_cross::Resource resource, VkDescriptorType descType);
  VkVertexInputBindingDescription   vertInputBindingDescFromResource(uint32_t binding, uint32_t stride);
  VkVertexInputAttributeDescription vertInputAttrDescFromResource(spirv_cross::Resource resource, size_t offset);
  void                              descSetLayoutFromBindings(std::vector<VkDescriptorSetLayoutBinding> bindings);
  
  std::function<bool(const spirv_cross::Resource&, const spirv_cross::Resource&)> stageInputComparator();

  static size_t sizeOfType(uint32_t typeId);

  VkDevice                     device;
  spirv_cross::CompilerGLSL    glsl;
  VkShaderStageFlags           stageFlags;
  SpirV                        spirv;
  spirv_cross::ShaderResources resources;

  VkDescriptorSetLayout                          descSetLayout;
  std::vector<VkDescriptorSetLayoutBinding>      descSetLayoutBindings;
  std::vector<VkVertexInputBindingDescription>   vertInputBindingDescs;
  std::vector<VkVertexInputAttributeDescription> vertInputAttrDescs;
};
