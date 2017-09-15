#include <NvFlex.h>
#include <json.hpp>
#include "magma.h"
#include "magma_context.h"
#include "math.h"
#include "fluid.h"
#include "fluid_simulation.h"
#include "fluid_pipeline.h"

Fluid::Fluid(Scene* scene, const ConfigNode& fluidObj, const MagmaContext* context)
  : SceneElement(scene, fluidObj, context), fluidSimulation(context->fluidSimulation) {
  fromJSON(fluidObj);
  pipeline = new FluidPipeline(context->graphics, scene, this);

  // TODO: Pipeline should be set downstream
  context->graphics->setPipeline(pipeline);
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
  int phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);
  uint32_t i = 0;
  for (float x = bln.x; x < trf.x; x += spacing) {
    for (float y = bln.y; y < trf.y; y += spacing) {
      for (float z = bln.z; z < trf.z; z += spacing) {
        glm::vec3 pos    = glm::vec3(x, y, z) + Math::randUnitVec() *  0.005f;
        positions[i]     = glm::vec4(pos.x, pos.y, pos.z, 1.0f);
        velocities[i]    = glm::vec3(0);
        phases[i]        = phase;
        activeIndices[i] = i++;
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

  positions     = new glm::vec4[particleCount];
  velocities    = new glm::vec3[particleCount];
  phases        = new int[particleCount];
  activeIndices = new int[particleCount];

  // Spawn particles
  createParticleGrid(minPos, maxPos, spacing);

  // Init solver
  solver = NvFlexCreateSolver(fluidSimulation->getLibrary(), particleCount, 0);

  // Allocate buffers
  particleBuffer = new NvFlexVector<glm::vec4>(fluidSimulation->getLibrary(), particleCount);
  velocityBuffer = new NvFlexVector<glm::vec3>(fluidSimulation->getLibrary(), particleCount);
  phaseBuffer    = new NvFlexVector<int>(fluidSimulation->getLibrary(), particleCount);
  activeBuffer   = new NvFlexVector<int>(fluidSimulation->getLibrary(), particleCount);

  // Map buffers
  particleBuffer->map();
  velocityBuffer->map();
  phaseBuffer->map();
  activeBuffer->map();

  particleBuffer->assign(positions, particleCount);
  velocityBuffer->assign(velocities, particleCount);
  phaseBuffer->assign(phases, particleCount);
  activeBuffer->assign(activeIndices, particleCount);

  // Unmap buffers
  particleBuffer->unmap();
  velocityBuffer->unmap();
  phaseBuffer->unmap();
  activeBuffer->unmap();

  // Write to device (async)
  NvFlexSetParticles(solver, particleBuffer->buffer, particleCount);
  NvFlexSetVelocities(solver, velocityBuffer->buffer, particleCount);
  NvFlexSetPhases(solver, phaseBuffer->buffer, particleCount);  
  NvFlexSetActive(solver, activeBuffer->buffer, particleCount);

  nlohmann::json jFluidProps = jsonObj["fluidProps"];

  params = defaultFluidParams;
  params.adhesion             = jFluidProps["adhesion"];
  params.buoyancy             = jFluidProps["buoyancy"];
  params.cohesion             = jFluidProps["cohesion"];
  params.radius = radius      = jFluidProps["radius"];
  params.viscosity            = jFluidProps["viscosity"];
  params.vorticityConfinement = jFluidProps["vorticityConfinement"];

  NvFlexSetParams(solver, &params);
}

void Fluid::update() {
  SceneElement::update();

  NvFlexUpdateSolver(solver, SPH_TIME_STEP, 1, false);
  
  NvFlexGetParticles(solver, particleBuffer->buffer, particleCount);

  particleBuffer->map(eNvFlexMapWait);
  particleBuffer->copyto(positions, particleCount);
  particleBuffer->unmap();
}

void Fluid::cleanup() {
  delete particleBuffer;
  delete velocityBuffer;
  delete phaseBuffer;
  delete activeBuffer;

  delete[] positions;
  delete[] velocities;
  delete[] phases;
  delete[] activeIndices;
}
