#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "magma_types.h"

#define CAMERA_FOV     45.f
#define CAMERA_UP      { 0, 1, 0 }
#define CAMERA_FORWARD { 0, 0, -1 }
#define CAMERA_NEAR     0.1f
#define CAMERA_FAR      1000.f
#define CAMERA_RATIO    16 / 9.f

struct Camera {
public:
  Camera(const ConfigNode& cameraObj);

  void parse(const ConfigNode& cameraObj);
  
  void onViewportChange(const Viewport& viewport);
  
  glm::mat4 getViewMatrix();
  glm::mat4 getProjectionMatrix();

  void dolly(float delta);
  void rotate(float dx, float dy);
  void pan(float dx, float dy);
  void orbit(glm::vec3 target, float dx, float dy);

  glm::vec3  pos;
  float      yaw = 0;
  float      pitch = 0;

private:
  void updateProjMatrix();
  void updateViewMatrix();

  float ratio = CAMERA_RATIO;
  
  glm::mat4 proj;
  glm::mat4 view;
};