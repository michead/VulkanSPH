#include "sph.h"
#include "math.h"
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

  // Apply params to solver
  NvFlexSetParams(solver, &params);
}

void SPH::loadParamsFromJson() {
  assert(paramsFilename);
  
  std::ifstream   i(paramsFilename);
  nlohmann::json  j;
  i >> j;

  params = {};
  params.adhesion             = j["fluidProps"]["adhesion"];
  params.buoyancy             = j["fluidProps"]["buoyancy"];
  params.cohesion             = j["fluidProps"]["cohesion"];
  params.radius               = j["fluidProps"]["radius"];
  params.viscosity            = j["fluidProps"]["viscosity"];
  params.vorticityConfinement = j["fluidProps"]["vorticityConfinement"];

  if (j["fluidProps"]["particles"].is_object()) {
    nlohmann::json jsonParticles = j["fluidProps"]["particles"];
    assert(jsonParticles["positions"].size()
        == jsonParticles["velocities"].size()
        == jsonParticles["phases"].size());
    // TODO: Copy particle attributes
  }

  // Particle grid for testing purposes
  particles.count = SPH::countParticlesInGrid(glm::vec3(-1), glm::vec3(1), 0.1f);

  // Initialize library and solver
  library = NvFlexInit();
  solver  = NvFlexCreateSolver(library, particles.count, 0);

  // Allocate buffers
  particleBuffer = NvFlexAllocBuffer(library, particles.count, sizeof(glm::vec4), eNvFlexBufferHost);
  velocityBuffer = NvFlexAllocBuffer(library, particles.count, sizeof(glm::vec4), eNvFlexBufferHost);
  phaseBuffer    = NvFlexAllocBuffer(library, particles.count, sizeof(int),       eNvFlexBufferHost);

  // Map buffers
  particles.positions  = (glm::vec4*)NvFlexMap(particleBuffer, eNvFlexMapWait);
  particles.velocities = (glm::vec3*)NvFlexMap(velocityBuffer, eNvFlexMapWait);
  particles.phases     = (int*)      NvFlexMap(phaseBuffer,    eNvFlexMapWait);

  // Set all particles as active
  activeBuffer = NvFlexAllocBuffer(library, particles.count, sizeof(int), eNvFlexBufferHost);
  activeIndices = (int*) NvFlexMap(activeBuffer, eNvFlexMapWait);
  std::iota(activeIndices, activeIndices + particles.count, 0);
  NvFlexUnmap(activeBuffer);
  NvFlexSetActive(solver, activeBuffer, particles.count);

  // Spawn particles
  SPH::createParticleGrid(&particles, glm::vec3(-1), glm::vec3(1), 0.1f);

  // unmap buffers
  NvFlexUnmap(particleBuffer);
  NvFlexUnmap(velocityBuffer);
  NvFlexUnmap(phaseBuffer);

  // write to device (async)
  NvFlexSetParticles(solver,  particleBuffer, particles.count);
  NvFlexSetVelocities(solver, velocityBuffer, particles.count);
  NvFlexSetPhases(solver,     phaseBuffer,    particles.count);
}

void SPH::draw() {

}

void SPH::update(double dt) {

}

uint32_t SPH::countParticlesInGrid(glm::vec3 bln, glm::vec3 trf, float radius) {
  uint32_t i = 0;
  for (int x = int(bln.x); x < trf.x; x++) {
    for (int y = int(bln.y); y < trf.y; y++) {
      for (int z = int(bln.z); z < trf.z; z++) {
        i++;
      }
    }
  }
  return i;
}

void SPH::createParticleGrid(Particles* particles, glm::vec3 bln, glm::vec3 trf, float radius) {
  uint32_t i = 0;
  for (int x = int(bln.x); x < trf.x; x++) {
    for (int y = int(bln.y); y < trf.y; y++) {
      for (int z = int(bln.z); z < trf.z; z++) {
        glm::vec3 pos = bln + glm::vec3(float(x), float(y), float(z)) * radius + randUnitVec() *  0.005f;
        particles->positions[i]  = glm::vec4(pos.x, pos.y, pos.z, 1.0f);
        particles->velocities[i] = glm::vec3(0);
        particles->phases[i]     = eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid;
        i++;
      }
    }
  }
}