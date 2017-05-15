#pragma once
#include <flex.h>
#include <glm\glm.hpp>
#include <vector>
#include "scene_element.h"

class SPH : public SceneElement {
public:
  SPH(const char* paramsFilename);
  ~SPH();

  virtual void draw();
  void update(float dt);

private:
  void init();
  void loadParamsFromJson();

  FlexSolver* solver;
  FlexParams  params;
  const char* paramsFilename;

  std::vector<glm::vec4>  particles;
  std::vector<glm::vec3>  velocities;
  std::vector<int>        phases;
};