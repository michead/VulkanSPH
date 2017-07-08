#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <json.hpp>

#define CAMERA_FOV   45.f
#define CAMERA_UP    { 0, 1, 0 }
#define CAMERA_NEAR  0.1f
#define CAMERA_FAR   1000.f
#define CAMERA_RATIO 16 / 9.f

struct Camera {
  Camera(const nlohmann::json& cameraObj) : ratio(ratio) { fromJSON(cameraObj); }

private:
  void updateMatrix() {
    glm::mat4 viewMat = glm::lookAt(pos, to, CAMERA_UP);
    glm::mat4 projMat = glm::perspective(CAMERA_FOV, ratio, CAMERA_NEAR, CAMERA_FAR);
    matrix = projMat * viewMat;
  }

  float     ratio = CAMERA_RATIO;
  glm::vec3 pos;
  glm::vec3 to;
  glm::mat4 matrix;

public:
  void fromJSON(const nlohmann::json& cameraObj) {
    const nlohmann::json jFrom = cameraObj["from"];
    const nlohmann::json jTo = cameraObj["to"];
    pos = { jFrom[0], jFrom[1], jFrom[2] };
    to = { jTo[0], jTo[1], jTo[2] };

    updateMatrix();
  }

  void updateRatio(float ratio) {
    this->ratio = ratio;
  }

  glm::mat4 getMatrix() const {
    return matrix;
  }
};