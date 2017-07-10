#pragma once

#include <glm\glm.hpp>

namespace Math {
  inline glm::vec3 randUnitVec() {
    glm::vec3 vec = {
      (float)rand() / (RAND_MAX),
      (float)rand() / (RAND_MAX),
      (float)rand() / (RAND_MAX) };
    return vec / float(vec.length());
  }

  inline glm::vec3 direction(float yaw, float pitch) {
    glm::vec3 direction;
    direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    direction.y = sin(glm::radians(pitch));
    direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    return direction;
  }
}
