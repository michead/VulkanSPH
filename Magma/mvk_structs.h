#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <map>
#include <vector>
#include <vulkan\vulkan.hpp>

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
  uint32_t vertexCount;
  glm::vec4* positions;
};

struct Light {
  glm::vec3 pos;
  glm::vec4 ke;
};

struct MVkVertexShaderUniformParticle {
  float     particleSize;
  glm::mat4 mvp;
};

struct MVkFragmentShaderUniformParticle {
  float              particleSize;
  glm::vec4          fluidDiffuse;
  glm::vec4          ambientColor;
  glm::mat4          mvp;
  glm::mat4          invMvp;
  glm::ivec4         viewport;
  glm::uint8_t       lightCount;
  std::vector<Light> lights;
};

struct MVkUniformBuffer {
  VkBuffer               buffer;
  VkDeviceSize           allocSize;
  VkDeviceMemory         deviceMemory;
  void*                  mappedMemory;
  VkDescriptorBufferInfo bufferInfo;
};

struct MVKPipeline {
  VkDescriptorSet                              descriptorSet;
  VkDescriptorSetLayout                        descriptorSetLayout;
  VkPipelineLayout                             layout;
  VkPipelineVertexInputStateCreateInfo         vertexInputState;
  VkPipelineInputAssemblyStateCreateInfo       inputAssemblyState;
  VkPipelineRasterizationStateCreateInfo       rasterizationState;
  VkPipelineColorBlendStateCreateInfo          colorBlendState;
  VkPipelineMultisampleStateCreateInfo         multisampleState;
  VkPipelineDynamicStateCreateInfo             dynamicState;
  VkPipelineViewportStateCreateInfo            viewportState;
  VkPipelineDepthStencilStateCreateInfo        depthStencilState;
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  VkRenderPass                                 renderPass;
  VkPipelineCache                              cache;
  VkPipeline                                   handle;
};

typedef std::map<std::string, std::map<std::string, std::vector<char>>> MVkShaderMap;

extern const VkClearValue                                   MVkClearValueColorWhite;
extern const VkClearValue                                   MVkClearValueDepthStencilOneZero;
extern const VkAttachmentDescription                        MVkBaseAttachmentColor;
extern const VkAttachmentDescription                        MVkBaseAttachmentDepth;
extern const VkAttachmentReference                          MVkBaseAttachmentColorReference;
extern const VkAttachmentReference                          MVkBaseAttachmentDepthReference;
extern const VkSubpassDescription                           MVkBaseSubpass;
extern const std::vector<VkVertexInputBindingDescription>   MVkVertexInputBindingDescriptionsSPH;
extern const std::vector<VkVertexInputAttributeDescription> MVkVertexAttributeDescriptionsSPH;
extern const VkPipelineVertexInputStateCreateInfo           MVkPipelineVertexInputStateSPH;
extern const VkPipelineInputAssemblyStateCreateInfo         MVkPipelineInputAssemblyStateSPH;
extern const VkPipelineRasterizationStateCreateInfo         MVkPipelineRasterizationStateSPH;
extern const VkPipelineColorBlendAttachmentState            MVkPipelineColorBlendAttachmentStateSPH;
extern const VkPipelineColorBlendStateCreateInfo            MVkPipelineColorBlendStateSPH;
extern const VkPipelineMultisampleStateCreateInfo           MVkPipelieMultisampleStateSPH;
extern const std::vector<VkDynamicState>                    MVkDynamicStatesSPH;
extern const VkPipelineDynamicStateCreateInfo               MVkPipelineDynamicStateSPH;
extern const VkStencilOpState                               MVkStencilOpStateFrontSPH;
extern const VkStencilOpState                               MVkStencilOpStateBackSPH;
extern const VkPipelineDepthStencilStateCreateInfo          MVkPipelineDepthStencilStateSPH;
extern const VkDescriptorSetLayoutBinding                   MVkDescriptorSetLayoutBindingUniformBufferVS;
extern const VkDescriptorSetLayoutBinding                   MVkDescriptorSetLayoutBindingUniformBufferFS;
