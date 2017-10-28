#include "layout_reflection.h"
#include "logger.h"

LayoutReflection::LayoutReflection(SpirV spirv, VkShaderStageFlags stageFlags) : spirv(spirv), stageFlags(stageFlags), glsl(std::move(spirv)) {
  extractResourcesfromSpirV();
  extractUniformBufferDescSetLayout();
}

void LayoutReflection::extractResourcesfromSpirV() {
  resources = glsl.get_shader_resources();
}

VkDescriptorSetLayoutBinding LayoutReflection::descSetLayoutBindingFromResource(spirv_cross::Resource resource, VkDescriptorType descType) {
  return {
    glsl.get_decoration(resource.id, spv::DecorationBinding),
    descType,
    1,
    stageFlags,
    nullptr
  };
}

VkDescriptorSetLayoutCreateInfo LayoutReflection::descSetLayoutFromBindings(std::vector<VkDescriptorSetLayoutBinding> bindings) {
  return {
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    nullptr,
    0,
    bindings.size(),
    bindings.data()
  };
}

void LayoutReflection::extractUniformBufferDescSetLayout() {
  for (auto &resource : resources.uniform_buffers) {
    logger->info("Extrapolating descriptor set layout binding for uniform buffer {0}", resource.name);
    descSetLayoutBindings.push_back(descSetLayoutBindingFromResource(resource, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
  }
  for (auto &resource : resources.sampled_images) {
    logger->info("Extrapolating descriptor set layout binding for sampled image {0}", resource.name);
    descSetLayoutBindings.push_back(descSetLayoutBindingFromResource(resource, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE));
  }
  descSetLayout = descSetLayoutFromBindings(descSetLayoutBindings);
}
