#include "layout_reflection.h"
#include "logger.h"

LayoutReflection::LayoutReflection(SpirV spirv) : spirv(spirv) {
  resources      = fromSpirV(spirv);
  descSetLayouts = extractUniformBufferDescSetLayouts();
}

spirv_cross::ShaderResources LayoutReflection::fromSpirV(SpirV spirv) {
  spirv_cross::CompilerGLSL glsl(std::move(spirv));
  return glsl.get_shader_resources();
}

VkDescriptorSetLayoutCreateInfo LayoutReflection::descSetLayoutFromUniformBuffer(spirv_cross::Resource resource) {
  return {
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, // sType
    nullptr,                                             // pNext
    0,                                                   // flags
    0,                                                   // bindingCount
    nullptr                                              // pBindings
  };
}

std::vector<VkDescriptorSetLayoutCreateInfo> LayoutReflection::extractUniformBufferDescSetLayouts() {
  std::vector<VkDescriptorSetLayoutCreateInfo> descSetLayouts;
  for (auto &resource : resources.uniform_buffers) {
    logger->info("Extrapolating descriptor set layout for uniform {0}", resource.name);
    descSetLayouts.push_back(descSetLayoutFromUniformBuffer(resource));
  }
  return descSetLayouts;
}
