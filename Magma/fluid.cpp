#include <NvFlex.h>
#include <json.hpp>
#include "magma.h"
#include "math.h"
#include "fluid.h"

Fluid::Fluid(Scene* scene, const nlohmann::json& jsonObj, MVkContext* context) : SceneElement(scene, jsonObj, context) {
  fromJSON(jsonObj);
  pipeline = new MVkPipeline(context, scene->camera, this);
};

uint32_t Fluid::countParticlesInGrid(glm::vec3 bln, glm::vec3 trf, float radius) {
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

void Fluid::createParticleGrid(glm::vec3 bln, glm::vec3 trf, float radius) {
  uint32_t i = 0;
  for (int x = int(bln.x); x < trf.x; x++) {
    for (int y = int(bln.y); y < trf.y; y++) {
      for (int z = int(bln.z); z < trf.z; z++) {
        glm::vec3 pos = bln + glm::vec3(float(x), float(y), float(z)) * radius + Math::randUnitVec() *  0.005f;
        positions[i]  = glm::vec4(pos.x, pos.y, pos.z, 1.0f);
        velocities[i] = glm::vec3(0);
        phases[i]     = eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid;
        i++;
      }
    }
  }
}

void Fluid::fromJSON(const nlohmann::json& jsonObj) {
  nlohmann::json jMinPos = jsonObj["min"];
  nlohmann::json jMaxPos = jsonObj["max"];
  glm::vec3 minPos = { jMinPos[0], jMinPos[1], jMinPos[2] };
  glm::vec3 maxPos = { jMaxPos[0], jMaxPos[1], jMaxPos[2] };

  float spacing = jsonObj["spacing"];

  // Particle grid for testing purposes
  particleCount = Fluid::countParticlesInGrid(minPos, maxPos, spacing);

  positions  = (glm::vec4*)malloc(particleCount * sizeof(glm::vec4));
  velocities = (glm::vec3*)malloc(particleCount * sizeof(glm::vec3));
  phases     = (int*)malloc(particleCount * sizeof(int));

  solver = NvFlexCreateSolver(flexLibrary, particleCount, 0);

  // Allocate buffers
  particleBuffer = NvFlexAllocBuffer(flexLibrary, particleCount, sizeof(glm::vec4), eNvFlexBufferHost);
  velocityBuffer = NvFlexAllocBuffer(flexLibrary, particleCount, sizeof(glm::vec4), eNvFlexBufferHost);
  phaseBuffer = NvFlexAllocBuffer(flexLibrary, particleCount, sizeof(int), eNvFlexBufferHost);

  // Map buffers
  positions  = (glm::vec4*)NvFlexMap(particleBuffer, eNvFlexMapWait);
  velocities = (glm::vec3*)NvFlexMap(velocityBuffer, eNvFlexMapWait);
  phases     = (int*)NvFlexMap(phaseBuffer, eNvFlexMapWait);

  // Set all particles as active
  activeBuffer = NvFlexAllocBuffer(flexLibrary, particleCount, sizeof(int), eNvFlexBufferHost);
  activeIndices = (int*)NvFlexMap(activeBuffer, eNvFlexMapWait);
  std::iota(activeIndices, activeIndices + particleCount, 0);
  NvFlexUnmap(activeBuffer);
  NvFlexSetActive(solver, activeBuffer, particleCount);

  // Spawn particles
  createParticleGrid(minPos, maxPos, spacing);

  // unmap buffers
  NvFlexUnmap(particleBuffer);
  NvFlexUnmap(velocityBuffer);
  NvFlexUnmap(phaseBuffer);

  // write to device (async)
  NvFlexSetParticles(solver, particleBuffer, particleCount);
  NvFlexSetVelocities(solver, velocityBuffer, particleCount);
  NvFlexSetPhases(solver, phaseBuffer, particleCount);

  nlohmann::json jFluidProps = jsonObj["fluidProps"];
  params = {};
  params.adhesion  = jFluidProps["adhesion"];
  params.buoyancy  = jFluidProps["buoyancy"];
  params.cohesion  = jFluidProps["cohesion"];
  params.radius    = jFluidProps["radius"];
  params.viscosity = jFluidProps["viscosity"];
  params.vorticityConfinement = jFluidProps["vorticityConfinement"];
  NvFlexSetParams(solver, &params);
}

void Fluid::cleanup() {
  delete positions;
  delete velocities;
  delete phases;
}
