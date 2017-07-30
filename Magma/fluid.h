#pragma once

#include <NvFlexExt.h>
#include "glm\glm.hpp"
#include "scene_element.h"
#include "magma_types.h"

#define SPH_TIME_STEP (1 / 60.f)

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
struct MagmaContext;

struct Fluid : public SceneElement {
  Fluid(Scene* scene, const ConfigNode& jsonObj, const MagmaContext* context);
  ~Fluid() { cleanup(); }

  virtual void fromJSON(const nlohmann::json& jsonObj) override;
  virtual void update() override;

  NvFlexSolver* getSolver();

  static uint32_t countParticlesInGrid(glm::vec3 min, glm::vec3 max, float radius);
  void createParticleGrid(glm::vec3 min, glm::vec3 max, float radius);

  uint32_t   particleCount;
  glm::vec4* positions;
  glm::vec3* velocities;
  int*       phases;
  float      radius;

private:
  void cleanup();

  FluidSimulation* fluidSimulation;

  NvFlexSolver*  solver;
  NvFlexParams   params;
  NvFlexVector<glm::vec4>* particleBuffer;
  NvFlexVector<glm::vec3>* velocityBuffer;
  NvFlexVector<int>* phaseBuffer;
  NvFlexVector<int>* activeBuffer;

  int* activeIndices;
};
