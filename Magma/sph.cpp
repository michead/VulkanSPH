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
  flexSetParticles( solver, (const float*)particles.positions.data(),  int(particles.size()), eFlexMemoryHost);
  flexSetVelocities(solver, (const float*)particles.velocities.data(), int(particles.size()), eFlexMemoryHost);
  flexSetPhases(    solver,               particles.phases.data(),     int(particles.size()), eFlexMemoryHost);

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
  params.mAdhesion              = j["fluidProps"]["adhesion"];
  params.mBuoyancy              = j["fluidProps"]["buoyancy"];
  params.mCohesion              = j["fluidProps"]["cohesion"];
  params.mRadius                = j["fluidProps"]["radius"];
  params.mViscosity             = j["fluidProps"]["viscosity"];
  params.mVorticityConfinement  = j["fluidProps"]["vorticityConfinement"];

  particles.clear();

  if (j["fluidProps"]["particles"].is_object()) {
    nlohmann::json jsonParticles = j["fluidProps"]["particles"];
    assert(jsonParticles["positions"].size()  ==
           jsonParticles["velocities"].size() ==
           jsonParticles["phases"].size());
    particles.resize(jsonParticles["positions"].size());
    // TODO: Copy particle attributes
  }
}

void SPH::draw() {

}

void SPH::update(double dt) {

}