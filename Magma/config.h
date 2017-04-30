#pragma once

#include "magma.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <glm\glm.hpp>
#include <simpleini\SimpleIni.h>
#include "logger.h"

struct Config{
  glm::ivec2 resolution;
  
  static Config& load(const char* iniConfig) {
    static Config config;

    CSimpleIniA ini(true, false, false);
    ini.LoadFile(iniConfig);

    std::stringstream sx(ini.GetValue("resolution", "x", "1920"));
    std::stringstream sy(ini.GetValue("resolution", "y", "1080"));
    sx >> config.resolution.x;
    sy >> config.resolution.y;

    return config;
  }
};