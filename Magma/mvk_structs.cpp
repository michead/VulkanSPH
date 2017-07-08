#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>

#include <glm\glm.hpp>
#include "mvk_structs.h"

const VkAttachmentDescription MVkBaseAttachmentColor = {
  0,
  VK_FORMAT_B8G8R8A8_UNORM,
  VK_SAMPLE_COUNT_1_BIT,
  VK_ATTACHMENT_LOAD_OP_CLEAR,
  VK_ATTACHMENT_STORE_OP_STORE,
  VK_ATTACHMENT_LOAD_OP_DONT_CARE,
  VK_ATTACHMENT_STORE_OP_DONT_CARE,
  VK_IMAGE_LAYOUT_UNDEFINED,
  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};

const VkAttachmentDescription MVkBaseAttachmentDepth = {
  0,
  VK_FORMAT_D16_UNORM,
  VK_SAMPLE_COUNT_1_BIT,
  VK_ATTACHMENT_LOAD_OP_CLEAR,
  VK_ATTACHMENT_STORE_OP_STORE,
  VK_ATTACHMENT_LOAD_OP_LOAD,
  VK_ATTACHMENT_STORE_OP_STORE,
  VK_IMAGE_LAYOUT_UNDEFINED,
  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
};

const VkAttachmentReference MVkBaseAttachmentColorReference = {
  0,
  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
};

const VkAttachmentReference MVkBaseAttachmentDepthReference = {
  1,
  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
};

const VkSubpassDescription MVkBaseSubpass = {
  0,
  VK_PIPELINE_BIND_POINT_GRAPHICS,
  0,
  nullptr,
  1,
  &MVkBaseAttachmentColorReference,
  nullptr,
  &MVkBaseAttachmentDepthReference,
  0,
  nullptr
};

const VkClearValue MVkClearValueColorWhite = {
  0,
  0,
  0,
  0
};

const VkClearValue MVkClearValueDepthStencilOneZero = {
  1,
  0
};

const std::vector<VkVertexInputBindingDescription> MVkVertexInputBindingDescriptionsSPH = { {
  0,
  sizeof(glm::vec4),
  VK_VERTEX_INPUT_RATE_VERTEX
}};

const std::vector<VkVertexInputAttributeDescription> MVkVertexAttributeDescriptionsSPH = {{
  0,
  0,
  VK_FORMAT_R32G32B32A32_SFLOAT,
  0
}};

const VkPipelineVertexInputStateCreateInfo  MVkPipelineVertexInputStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
  nullptr,
  0,
  MVkVertexInputBindingDescriptionsSPH.size(),
  MVkVertexInputBindingDescriptionsSPH.data(),
  MVkVertexAttributeDescriptionsSPH.size(),
  MVkVertexAttributeDescriptionsSPH.data()
};

const VkPipelineInputAssemblyStateCreateInfo MVkPipelineInputAssemblyStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
  nullptr,
  0,
  VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
  VK_TRUE
};

const VkPipelineRasterizationStateCreateInfo MVkPipelineRasterizationStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
  nullptr,
  0,
  VK_FALSE,
  VK_TRUE,
  VK_POLYGON_MODE_FILL,
  VK_CULL_MODE_NONE,
  VK_FRONT_FACE_COUNTER_CLOCKWISE,
  VK_FALSE,
  0,
  0,
  0,
  1.f
};

const VkPipelineColorBlendAttachmentState MVkPipelineColorBlendAttachmentStateSPH = {
  VK_FALSE,
  VK_BLEND_FACTOR_ONE,
  VK_BLEND_FACTOR_ZERO,
  VK_BLEND_OP_MAX,
  VK_BLEND_FACTOR_ONE,
  VK_BLEND_FACTOR_ZERO,
  VK_BLEND_OP_MAX,
  VK_COLOR_COMPONENT_R_BIT |
  VK_COLOR_COMPONENT_G_BIT |
  VK_COLOR_COMPONENT_B_BIT |
  VK_COLOR_COMPONENT_A_BIT
};

const VkPipelineColorBlendStateCreateInfo MVkPipelineColorBlendStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
  nullptr,
  0,
  VK_FALSE,
  VK_LOGIC_OP_NO_OP,
  1,
  &MVkPipelineColorBlendAttachmentStateSPH,
  NULL
};

const VkPipelineMultisampleStateCreateInfo MVkPipelieMultisampleStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
  nullptr,
  0,
  VK_SAMPLE_COUNT_1_BIT,
  VK_TRUE,
  1.f,
  nullptr,
  VK_FALSE,
  VK_FALSE
};

const std::vector<VkDynamicState> MVkDynamicStatesSPH = {
  VK_DYNAMIC_STATE_VIEWPORT,
  VK_DYNAMIC_STATE_SCISSOR
};

const VkPipelineDynamicStateCreateInfo MVkPipelineDynamicStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
  nullptr,
  0,
  MVkDynamicStatesSPH.size(),
  MVkDynamicStatesSPH.data()
};

const VkStencilOpState MVkStencilOpStateFrontSPH, 
                       MVkStencilOpStateBackSPH = {
  VK_STENCIL_OP_KEEP,
  VK_STENCIL_OP_REPLACE,
  VK_STENCIL_OP_KEEP,
  VK_COMPARE_OP_LESS,
  0,
  0,
  0
};

const VkPipelineDepthStencilStateCreateInfo MVkPipelineDepthStencilStateSPH = {
  VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
  nullptr,
  0,
  VK_TRUE,
  VK_TRUE,
  VK_COMPARE_OP_LESS,
  VK_TRUE,
  VK_FALSE,
  MVkStencilOpStateFrontSPH,
  MVkStencilOpStateBackSPH,
  0.f,
  1.f,
};

const VkDescriptorSetLayoutBinding MVkDescriptorSetLayoutBindingUniformBufferVS = {
  0,
  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
  1,
  VK_SHADER_STAGE_VERTEX_BIT,
  nullptr
};

const VkDescriptorSetLayoutBinding MVkDescriptorSetLayoutBindingUniformBufferFS = {
  1,
  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
  1,
  VK_SHADER_STAGE_FRAGMENT_BIT,
  nullptr
};
