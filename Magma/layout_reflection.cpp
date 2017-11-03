#include <algorithm>
#include <numeric>
#include "layout_reflection.h"
#include "gfx_wrap.h"
#include "logger.h"

LayoutReflection::LayoutReflection(VkDevice device, SpirV spirv, VkShaderStageFlags stageFlags)
    : device(device), spirv(spirv), stageFlags(stageFlags), glsl(std::move(spirv)) {
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

VkVertexInputBindingDescription LayoutReflection::vertInputBindingDescFromResource(uint32_t binding, uint32_t stride) {
  return {
    binding,
    stride,
    VK_VERTEX_INPUT_RATE_VERTEX
  };
}

VkVertexInputAttributeDescription LayoutReflection::vertInputAttrDescFromResource(spirv_cross::Resource resource, size_t offset) {
  VkFormat format = VK_FORMAT_UNDEFINED;

  switch (resource.type_id) {
  case 13:
    format = VK_FORMAT_R32G32B32_SFLOAT;
    break;
  case 15:
  case 21:
    format = VK_FORMAT_R32G32_SFLOAT;
    break;
  case 30:
    format = VK_FORMAT_R32G32B32A32_SFLOAT;
    break;
  default:
    break;
  }

  if (format == VK_FORMAT_UNDEFINED) {
    logger->error("Shader resource type with id {0} not unrecognized", resource.type_id);
    exit(EXIT_FAILURE);
  }

  return {
    glsl.get_decoration(resource.id, spv::DecorationLocation),
    glsl.get_decoration(resource.id, spv::DecorationBinding),
    format,
    offset
  };
}

void LayoutReflection::descSetLayoutFromBindings(std::vector<VkDescriptorSetLayoutBinding> bindings) {
  GfxWrap::createDescriptorSetLayout(device, bindings, descSetLayout);
}

void LayoutReflection::extractUniformBufferDescSetLayout() {
  std::map<uint32_t, std::vector<uint32_t>> bindingToTypesMap;
  for (auto &resource : resources.stage_inputs) {
    logger->info("Extrapolating input attribute description {0}", resource.name);
    vertInputAttrDescs.push_back(vertInputAttrDescFromResource(resource, 0));
    
    uint32_t binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
    if (bindingToTypesMap.find(binding) != bindingToTypesMap.end()) {
      bindingToTypesMap.at(binding).push_back(resource.type_id);
    } else {
      bindingToTypesMap.insert(std::pair<uint32_t, std::vector<uint32_t>>(binding, { resource.type_id }));
    }
  }
  for (const auto& k : bindingToTypesMap) {
    std::vector<int> widths(k.second.size());
    std::transform(k.second.begin(), k.second.end(), widths.begin(), [&](uint32_t typeId) -> int {
      return static_cast<int>(glsl.get_type(typeId).width);
    });
    uint32_t stride = std::accumulate(widths.begin(), widths.end(), 0, std::plus<int>());
    vertInputBindingDescs.push_back(vertInputBindingDescFromResource(k.first, stride));
  }
  for (const auto &resource : resources.uniform_buffers) {
    logger->info("Extrapolating descriptor set layout binding for uniform buffer {0}", resource.name);
    descSetLayoutBindings.push_back(descSetLayoutBindingFromResource(resource, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
  }
  for (const auto &resource : resources.sampled_images) {
    logger->info("Extrapolating descriptor set layout binding for image sampler {0}", resource.name);
    descSetLayoutBindings.push_back(descSetLayoutBindingFromResource(resource, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
  }
  descSetLayoutFromBindings(descSetLayoutBindings);
}
