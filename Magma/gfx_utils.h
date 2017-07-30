#pragma once

#include <map>
#include <vector>
#include "logger.h"

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
}
