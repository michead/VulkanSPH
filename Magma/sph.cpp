#include "sph.h"
#include "json.hpp"
#include <cassert>
#include <fstream>

SPH::SPH(const char* paramsFilename) : paramsFilename(paramsFilename) {
  init();
}

SPH::~SPH() {

}

void SPH::init() {
  // Load fluid parameters from JSON config file
  loadParamsFromJson();

  // Initialize solver
  flexInit();
  solver = flexCreateSolver(int(particles.size()), 0);
  flexSetParams(solver, &params);
  
  // Set particles' initial attributes
  flexSetParticles(solver, &particles[0].x, int(particles.size()), eFlexMemoryHost);
  flexSetVelocities(solver, &velocities[0].x, int(particles.size()), eFlexMemoryHost);
  flexSetPhases(solver, phases.data(), int(particles.size()), eFlexMemoryHost);

  // Set all particles as active
  std::vector<int> activeIndices(particles.size());
  std::iota(activeIndices.begin(), activeIndices.end(), 0);
  flexSetActive(solver, activeIndices.data(), int(particles.size()), eFlexMemoryHost);
}

void SPH::loadParamsFromJson() {
  assert(paramsFilename);
  
  std::ifstream   i(paramsFilename);
  nlohmann::json  j;
  i >> j;

  params = {};
  params.mAdhesion              = j["adhesion"];
  params.mBuoyancy              = j["buoyancy"];
  params.mCohesion              = j["cohesion"];
  params.mRadius                = j["radius"];
  params.mViscosity             = j["viscosity"];
  params.mVorticityConfinement  = j["vorticityConfinement"];

  uint32_t particleCount = j["particleCount"];

  particles.clear();
  particles.resize(particleCount);

  velocities.clear();
  velocities.resize(particleCount);
}

void SPH::draw() {

}

void SPH::update(float dt) {

}