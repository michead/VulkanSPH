#include <vulkan\vulkan.hpp>
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
