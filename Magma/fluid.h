#pragma once

#include <NvFlex.h>
#include "glm\glm.hpp"
#include "scene_element.h"

struct FluidInitialState {
  glm::vec3 minPos;
  glm::vec3 maxPos;
  float spacing;
};

struct ParticleAttribs {

};

struct Particle {
  Particle(glm::vec4 position, glm::vec3 velocity, int phase) : position{ position }, velocity{ velocity }, phase{ phase } {}

  glm::vec4 position;
  glm::vec3 velocity;
  int       phase;
};

struct Scene;

struct Fluid : public SceneElement {
  Fluid(Scene* scene, const nlohmann::json& jsonObj, MVkContext* context);
  ~Fluid() { cleanup(); }

  virtual void fromJSON(const nlohmann::json& jsonObj);

  static uint32_t countParticlesInGrid(glm::vec3 min, glm::vec3 max, float radius);
  void createParticleGrid(glm::vec3 min, glm::vec3 max, float radius);

  void update(double dt);

  uint32_t   particleCount;
  glm::vec4* positions;
  glm::vec3* velocities;
  int*       phases;

private:
  void cleanup();

  NvFlexSolver*  solver;
  NvFlexParams   params;
  NvFlexBuffer*  particleBuffer;
  NvFlexBuffer*  velocityBuffer;
  NvFlexBuffer*  phaseBuffer;
  NvFlexBuffer*  activeBuffer;

  int*       activeIndices;
};
