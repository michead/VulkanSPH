#include <NvFlex.h>
#include <json.hpp>
#include "magma.h"
#include "magma_context.h"
#include "math.h"
#include "fluid.h"
#include "fluid_simulation.h"

Fluid::Fluid(Scene* scene, const ConfigNode& fluidObj, const MagmaContext* context)
  : SceneElement(scene, fluidObj, context), fluidSimulation(context->fluidSimulation) {
  fromJSON(fluidObj);
  pipeline = new MVkPipeline(context->graphics, scene->camera, this);
};

NvFlexSolver* Fluid::getSolver() {
  return solver;
}

uint32_t Fluid::countParticlesInGrid(glm::vec3 bln, glm::vec3 trf, float spacing) {
  uint32_t i = 0;
  for (float x = bln.x; x < trf.x; x += spacing) {
    for (float y = bln.y; y < trf.y; y += spacing) {
      for (float z = bln.z; z < trf.z; z += spacing) {
        i++;
      }
    }
  }
  return i;
}

void Fluid::createParticleGrid(glm::vec3 bln, glm::vec3 trf, float spacing) {
  uint32_t i = 0;
  for (float x = bln.x; x < trf.x; x += spacing) {
    for (float y = bln.y; y < trf.y; y += spacing) {
      for (float z = bln.z; z < trf.z; z += spacing) {
        glm::vec3 pos = glm::vec3(x, y, z) + Math::randUnitVec() *  0.005f;
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

  solver = NvFlexCreateSolver(fluidSimulation->getLibrary(), particleCount, 0);

  // Allocate buffers
  particleBuffer = NvFlexAllocBuffer(fluidSimulation->getLibrary(), particleCount, sizeof(glm::vec4), eNvFlexBufferHost);
  velocityBuffer = NvFlexAllocBuffer(fluidSimulation->getLibrary(), particleCount, sizeof(glm::vec4), eNvFlexBufferHost);
  phaseBuffer    = NvFlexAllocBuffer(fluidSimulation->getLibrary(), particleCount, sizeof(int), eNvFlexBufferHost);

  // Map buffers
  positions  = (glm::vec4*)NvFlexMap(particleBuffer, eNvFlexMapWait);
  velocities = (glm::vec3*)NvFlexMap(velocityBuffer, eNvFlexMapWait);
  phases     = (int*)NvFlexMap(phaseBuffer, eNvFlexMapWait);

  // Set all particles as active
  activeBuffer = NvFlexAllocBuffer(fluidSimulation->getLibrary(), particleCount, sizeof(int), eNvFlexBufferHost);
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
  params.adhesion             = jFluidProps["adhesion"];
  params.buoyancy             = jFluidProps["buoyancy"];
  params.cohesion             = jFluidProps["cohesion"];
  params.radius = radius      = jFluidProps["radius"];
  params.viscosity            = jFluidProps["viscosity"];
  params.vorticityConfinement = jFluidProps["vorticityConfinement"];
  NvFlexSetParams(solver, &params);
}

void Fluid::cleanup() {
  delete positions;
  delete velocities;
  delete phases;
}
