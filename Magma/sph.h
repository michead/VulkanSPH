#pragma once
#include <NvFlex.h>
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
  glm::vec4* positions;
  glm::vec3* velocities;
  int*       phases;
  size_t     count;

  Particle& operator[] (int i) {
    Particle particle{ positions[i], velocities[i], phases[i] };
    return   particle;
  }
};

class SPH : public SceneElement {
public:
  SPH(const char* paramsFilename);
  ~SPH();

  virtual void draw();
  void update(double dt);

  Particles getParticles() { return particles; }

  static uint32_t countParticlesInGrid(glm::vec3 bln, glm::vec3 trf, float radius);
  static void createParticleGrid(Particles* particles, glm::vec3 bln, glm::vec3 trf, float radius);

private:
  void init();
  void loadParamsFromJson();

  NvFlexLibrary* library;
  NvFlexSolver*  solver;
  NvFlexParams   params;
  NvFlexBuffer*  particleBuffer;
  NvFlexBuffer*  velocityBuffer;
  NvFlexBuffer*  phaseBuffer;
  NvFlexBuffer*  activeBuffer;

  int*           activeIndices;

  const char*    paramsFilename;

  Particles      particles;
};