#include "camera.h"
#include "math.h"
#include "magma_context.h"

Camera::Camera(const ConfigNode& cameraObj) {
  parse(cameraObj);

  updateProjMatrix();
  updateViewMatrix();
}

void Camera::parse(const ConfigNode& cameraObj) {
  ConfigNode _pos = cameraObj["pos"];
  assert( _pos.size() == 3);
  pos = { _pos[0], _pos[1], _pos[2] };
  
  yaw   = cameraObj["yaw"];
  pitch = cameraObj["pitch"];
}

void Camera::onViewportChange(const Viewport& viewport) {
  ratio = viewport.width / viewport.height;
}

void Camera::rotate(float dx, float dy) {
  yaw += dx;
  pitch = glm::clamp<float>(pitch + dy, -89, 89);
}

void Camera::dolly(float delta) {
  // TODO
}

void Camera::pan(float dx, float dy) {
  // TODO
}

void Camera::orbit(glm::vec3 target, float dx, float dy) {
  // TODO
}

void Camera::updateProjMatrix() {
  proj = glm::perspective(glm::radians(CAMERA_FOV), ratio, CAMERA_NEAR, CAMERA_FAR);
  proj[1][1] *= -1;
}

void Camera::updateViewMatrix() {
  // TODO: Yaw and pitch are left unused
  view = glm::lookAt(pos, glm::vec3(0), CAMERA_UP);
}

glm::mat4 Camera::getViewMatrix() {
  updateViewMatrix();
  return view;
}

glm::mat4 Camera::getProjectionMatrix() {
  updateProjMatrix();
  return proj;
}
