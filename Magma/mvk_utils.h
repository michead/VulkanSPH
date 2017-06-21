#pragma once

#include <map>
#include <vector>
#include "logger.h"

namespace MVkUtils {
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
}
