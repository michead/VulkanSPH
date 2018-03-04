#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <map>
#include <vector>
#include <vulkan\vulkan.hpp>
#include "camera.h"
#include "light.h"

#define MAX_NUM_LIGHTS 32

struct MVkAttachment {
  VkImage       image;
  VkImageView   imageView;
  VkImageLayout imageLayout;
  VkSampler     sampler;
};

struct MVkFramebuffer {
  VkFramebuffer              handle;
  VkRenderPass               renderPass;
  std::vector<MVkAttachment> attachments;
};

struct MVkSwapchain {
  VkSwapchainKHR           handle;
  VkExtent2D               extent;
  std::vector<VkImage>     images;
  std::vector<VkImageView> imageViews;
};

struct MVkWsi {
  VkSurfaceKHR             surface;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
};

struct MVkShaderStage {
  VkPipelineShaderStageCreateInfo info;
  std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
  std::vector<VkVertexInputBindingDescription> vertexInputBindings;
  std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
};

struct MVkShaderProgram {
public:
  std::vector<VkDescriptorSetLayoutBinding> getLayoutBindings() const {
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    if (bVert) layoutBindings.insert(layoutBindings.end(), vert.layoutBindings.begin(), vert.layoutBindings.end());
    if (bGeom) layoutBindings.insert(layoutBindings.end(), geom.layoutBindings.begin(), geom.layoutBindings.end());
    if (bFrag) layoutBindings.insert(layoutBindings.end(), frag.layoutBindings.begin(), frag.layoutBindings.end());
    return layoutBindings;
  }

  std::vector<VkVertexInputBindingDescription> getVertexInputBindings() const {
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    if (bVert) {
      vertexInputBindings.insert(vertexInputBindings.end(), vert.vertexInputBindings.begin(), vert.vertexInputBindings.end());
    }
    return vertexInputBindings;
  }

  std::vector<VkVertexInputAttributeDescription> getVertexInputAttributes() const {
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    if (bVert) {
      vertexInputAttributes.insert(vertexInputAttributes.end(), vert.vertexInputAttributes.begin(), vert.vertexInputAttributes.end());
    }
    return vertexInputAttributes;
  }

  std::vector<VkPipelineShaderStageCreateInfo> getStages() const {
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    if (bVert) stages.push_back(vert.info);
    if (bGeom) stages.push_back(geom.info);
    if (bFrag) stages.push_back(frag.info);
    return stages;
  }

  VkPipelineVertexInputStateCreateInfo getVertexInputState() const {
    return {
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        nullptr,
        0,
        getVertexInputBindings().size(),
        getVertexInputBindings().data(),
        getVertexInputAttributes().size(),
        getVertexInputAttributes().data()
    };
  }

  void setVert(MVkShaderStage vert) {
    this->vert = vert;
    bVert = true;
  }

  void setGeom(MVkShaderStage geom) {
    this->geom = geom;
    bGeom = true;
  }

  void setFrag(MVkShaderStage frag) {
    this->frag = frag;
    bFrag = true;
  }

private:
  MVkShaderStage vert;
  MVkShaderStage geom;
  MVkShaderStage frag;

  bool bVert = false;
  bool bGeom = false;
  bool bFrag = false;
};

struct MVkPipelineParams {
  uint8_t    numPasses;
  uint32_t   vertexCount;
  glm::vec4* positions;
};

struct MVkVert0 {
  glm::mat4 view;
  glm::mat4 proj;
  float     particleSize;
};

struct MVkFrag0 {
  glm::vec4          fluidDiffuse;
  glm::vec4          ambientColor;
  glm::mat4          proj;
  glm::mat4          invProj;
  glm::ivec4         viewport;
  float              particleSize;
  unsigned int       lightCount;
  glm::vec4          lightPos[MAX_NUM_LIGHTS];
};

struct MVkVert1 {
  // None
};

struct MVkFrag1 {
  MVkAttachment depthBuffer;
};

struct MVkVert2 {
  // None
};

struct MVkFrag2 {
  glm::mat4 invProj;
};

struct MVkQuadVertexAttribute {
  glm::vec4 pos;
  glm::vec2 texCoord;
};

struct MVkBufferDesc {
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

typedef std::map<std::string, std::map<std::string, std::vector<uint32_t>>> MVkShaderMap;

extern const VkClearValue                                   MVkClearValueColorWhite;
extern const VkClearValue                                   MVkClearValueDepthStencilOneZero;
extern const VkAttachmentDescription                        MVkBaseAttachmentColor;
extern const VkAttachmentDescription                        MVkBaseAttachmentDepth;
extern const std::vector<VkAttachmentDescription>           MVkBaseAttachments;
extern const VkAttachmentReference                          MVkBaseAttachmentColorReference;
extern const VkAttachmentReference                          MVkBaseAttachmentDepthReference;
extern const VkAttachmentReference                          MVkBaseInputAttachmentReference;
extern const VkSubpassDescription                           MVkBaseColorSubpass;
extern const VkSubpassDescription                           MVkBaseDepthSubpass;
extern const VkSubpassDescription                           MVkBaseColorDepthSubpass;
extern const std::vector<VkSubpassDependency>               MVkBaseDependencies;
extern const std::vector<VkVertexInputBindingDescription>   MVkVertexInputBindingDescriptionsSPH;
extern const std::vector<VkVertexInputAttributeDescription> MVkVertexAttributeDescriptionsSPH;
extern const VkPipelineVertexInputStateCreateInfo           MVkPipelineVertexInputStateSPH;
extern const VkPipelineInputAssemblyStateCreateInfo         MVkPipelineInputAssemblyStatePointList;
extern const VkPipelineInputAssemblyStateCreateInfo         MVkPipelineInputAssemblyStateTriangleList;
extern const VkPipelineRasterizationStateCreateInfo         MVkPipelineRasterizationStateSPH;
extern const VkPipelineColorBlendAttachmentState            MVkPipelineColorBlendAttachmentStateSPH;
extern const VkPipelineColorBlendStateCreateInfo            MVkPipelineColorBlendStateSPH;
extern const VkPipelineMultisampleStateCreateInfo           MVkPipelineMultisampleStateSPH;
extern const std::vector<VkDynamicState>                    MVkDynamicStatesSPH;
extern const VkPipelineDynamicStateCreateInfo               MVkPipelineDynamicStateSPH;
extern const VkStencilOpState                               MVkStencilOpStateFrontSPH;
extern const VkStencilOpState                               MVkStencilOpStateBackSPH;
extern const VkPipelineDepthStencilStateCreateInfo          MVkPipelineDepthStencilStateOn;
extern const VkPipelineDepthStencilStateCreateInfo          MVkPipelineDepthStencilStateOff;
extern const VkDescriptorSetLayoutBinding                   MVkDescriptorSetLayoutBindingUniformBufferVS;
extern const VkDescriptorSetLayoutBinding                   MVkDescriptorSetLayoutBindingUniformBufferFS;
