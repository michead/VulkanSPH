#pragma once
#include <flex.h>
#include <glm\glm.hpp>
#include <vector>
#include "scene_element.h"

struct Particle {
  Particle(glm::vec4 position, glm::vec3 velocity, int phase) : position{position}, velocity{velocity}, phase{phase} {}

  glm::vec4 position;
  glm::vec3 velocity;
  int       phase;
};

struct Particles {
  std::vector<glm::vec4> positions;
  std::vector<glm::vec3> velocities;
  std::vector<int>       phases;

  Particle& operator[] (int i) {
    Particle particle{ positions[i], velocities[i], phases[i] };
    return   particle;
  }

  void clear() {
    positions.clear();
    velocities.clear();
    phases.clear();
  }

  void resize(size_t newSize) {
    positions.resize(newSize);
    velocities.resize(newSize);
    phases.resize(newSize);
  }

  size_t size() {
    assert(positions.size() == velocities.size() &&
          velocities.size() == phases.size());
    return positions.size();
  }
};

class SPH : public SceneElement {
public:
  SPH(const char* paramsFilename);
  ~SPH();

  virtual void draw();
  void update(double dt);

private:
  void init();
  void loadParamsFromJson();

  FlexSolver* solver;
  FlexParams  params;
  const char* paramsFilename;

  Particles particles;
};