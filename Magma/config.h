#pragma once
#include <cassert>
#include <fstream>
#include <sstream>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <simpleini\SimpleIni.h>
#include "magma.h"
#include "logger.h"

struct Config{
  glm::ivec2 resolution;
  
  static Config& load(const char* iniConfig) {
    static Config config;

    CSimpleIniA ini(true, false, false);
    ini.LoadFile(iniConfig);

    std::stringstream sx(ini.GetValue("resolution", "x", "1280"));
    std::stringstream sy(ini.GetValue("resolution", "y", "720"));
    sx >> config.resolution.x;
    sy >> config.resolution.y;

    return config;
  }
};