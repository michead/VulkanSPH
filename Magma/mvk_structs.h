#pragma once

#include "vulkan\vulkan.hpp"

struct MVkAttachment {
  VkImage image;
  VkImageView imageView;
};

struct MVkFramebuffer {
  VkFramebuffer handle;
  VkRenderPass renderPass;
  std::vector<MVkAttachment> attachments;
};

struct MVkSwapchain {
  VkSwapchainKHR handle;
  VkExtent2D     extent;
  std::vector<VkImage>     images;
  std::vector<VkImageView> imageViews;
};

struct MVkWsi {
  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
};

struct MVkPipelineParams {
  uint8_t numPasses;
};

struct MVKPipeline {
  VkPipelineLayout layout;
  VkPipelineVertexInputStateCreateInfo vertexInputState;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
  VkPipelineRasterizationStateCreateInfo rasterizationState;
  VkPipelineColorBlendStateCreateInfo colorBlendState;
  VkPipelineMultisampleStateCreateInfo multisampleState;
  VkPipelineDynamicStateCreateInfo dynamicState;
  VkPipelineViewportStateCreateInfo viewportState;
  VkPipelineDepthStencilStateCreateInfo depthStencilState;
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  VkRenderPass renderPass;
  VkPipelineCache cache;
  VkPipeline handle;
};

extern const VkClearValue             MVkClearValueColorWhite;
extern const VkClearValue             MVkClearValueDepthStencilOneZero;
extern const VkAttachmentDescription  MVkBaseAttachmentColor;
extern const VkAttachmentDescription  MVkBaseAttachmentDepth;
extern const VkAttachmentReference    MVkBaseAttachmentColorReference;
extern const VkAttachmentReference    MVkBaseAttachmentDepthReference;
extern const VkSubpassDescription     MVkBaseSubpass;
