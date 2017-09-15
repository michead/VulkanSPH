#pragma once
#include <glm\glm.hpp>
#include "magma_types.h"

struct Light {
  Light();
  Light(const ConfigNode& lightObj);

  glm::vec3 pos;
};
