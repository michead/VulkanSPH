#pragma once

#include <map>
#include <vector>
#include "gfx_context.h"
#include "gfx_structs.h"
#include "gfx_wrap.h"
#include "layout_reflection.h"
#include "logger.h"

const std::vector<MVkQuadVertexAttribute> fsQuadVertices = {
  { glm::vec4(-1.f, -1.f, 0.f, 1.f), glm::vec2(0, 0) },
  { glm::vec4(-1.f,  1.f, 0.f, 1.f), glm::vec2(0, 1) },
  { glm::vec4(1.f,  1.f, 0.f, 1.f),  glm::vec2(1, 1) },
  { glm::vec4(1.f, -1.f, 0.f, 1.f),  glm::vec2(1, 0) }
};

const std::vector<uint32_t> fsQuadIndices = {
  0, 2, 3,
  0, 1, 2
};

#define shader(context, shaderName, shaderStage) context->shaderMap[shaderName][shaderStage]
#define vert_shader(context, shaderName) shader(context, shaderName, "vert")
#define geom_shader(context, shaderName) shader(context, shaderName, "geom")
#define frag_shader(context, shaderName) shader(context, shaderName, "frag")
#define has_vert_shader(context, shaderName) ((vert_shader(context, shaderName)).size() > 0)
#define has_geom_shader(context, shaderName) ((geom_shader(context, shaderName)).size() > 0)
#define has_frag_shader(context, shaderName) ((frag_shader(context, shaderName)).size() > 0)

namespace GfxUtils {
  inline bool isGLSLFilename(const char* filename) {
    std::string extension = std::string(filename).substr(std::string(filename).find('.') + 1);
    return extension == "conf"
        || extension == "vert"
        || extension == "tesc"
        || extension == "tese"
        || extension == "geom"
        || extension == "frag"
        || extension == "comp";
  }
  inline VkPipelineViewportStateCreateInfo viewportState(const VkViewport* viewport,
                                                         const VkRect2D* scissor) {
    return {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      nullptr,
      0,
      1,
      viewport,
      1,
      scissor
    };
  }
  inline VkViewport viewport(uint32_t width, uint32_t height) {
    return {
      0,
      0,
      (float)width,
      (float)height,
      0.f,
      1.f
    };
  }
  inline VkRect2D scissor(VkExtent2D extent) {
    return {
      { 0, 0 },
      extent
    };
  }
  inline MVkShaderProgram buildShaderProgram(GfxContext* context, const char* shaderName) {
    VkShaderModule moduleVert;
    VkShaderModule moduleGeom;
    VkShaderModule moduleFrag;

    VkPipelineShaderStageCreateInfo createInfoVert;
    VkPipelineShaderStageCreateInfo createInfoGeom;
    VkPipelineShaderStageCreateInfo createInfoFrag;

    MVkShaderProgram program;

    if (has_vert_shader(context, shaderName)) {
      LayoutReflection reflect(context->device, vert_shader(context, shaderName), VK_SHADER_STAGE_VERTEX_BIT);
      GfxWrap::createShaderModule(context->device, vert_shader(context, shaderName), moduleVert);
      GfxWrap::shaderStage(moduleVert, VK_SHADER_STAGE_VERTEX_BIT, createInfoVert);
      program.setVert({
        createInfoVert,
        reflect.getDescriptorSetLayoutBindings(),
        reflect.getVertexInputBindingDescriptions(),
        reflect.getVertexInputAttributeDescriptions()
      });
    }

    if (has_geom_shader(context, shaderName)) {
      LayoutReflection reflect(context->device, geom_shader(context, shaderName), VK_SHADER_STAGE_GEOMETRY_BIT);
      GfxWrap::createShaderModule(context->device, geom_shader(context, shaderName), moduleGeom);
      GfxWrap::shaderStage(moduleFrag, VK_SHADER_STAGE_GEOMETRY_BIT, createInfoGeom);
      program.setGeom({ { }, { } }); /* Placeholder */
    }

    if (has_frag_shader(context, shaderName)) {
      LayoutReflection reflect(context->device, frag_shader(context, shaderName), VK_SHADER_STAGE_FRAGMENT_BIT);
      GfxWrap::createShaderModule(context->device, frag_shader(context, shaderName), moduleFrag);
      GfxWrap::shaderStage(moduleFrag, VK_SHADER_STAGE_FRAGMENT_BIT, createInfoFrag);
      program.setFrag({
        createInfoFrag,
        reflect.getDescriptorSetLayoutBindings()
      });
    }

    return program;
  }
}
