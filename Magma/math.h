#pragma once

#include <glm\glm.hpp>

inline glm::vec3 randUnitVec() {
  glm::vec3 vec = {
    (float) rand() / (RAND_MAX),
    (float) rand() / (RAND_MAX),
    (float) rand() / (RAND_MAX) };
  return vec / float(vec.length());
}
