#include "fluid_simulation.h"
#include "fluid.h"
#include "scene.h"
#include "scene_element.h"
#include "mesh.h"
#include "logger.h"

FluidSimulation::FluidSimulation() {
  library = NvFlexInit(NV_FLEX_VERSION, FluidSimulation::errorCallback);

  if (!library) {
    logger->error("Error encountered while initializing Flex.");
    exit(EXIT_FAILURE);
  }

  char deviceName[256];
  strcpy_s(deviceName, NvFlexGetDeviceName(library));
  logger->info("SPH compute device: {0}", deviceName);
}

FluidSimulation::~FluidSimulation() {
  cleanup();
}

NvFlexLibrary* FluidSimulation::getLibrary() {
  return library;
}

void FluidSimulation::initCollision(Scene* scene) {
  this->scene = scene;

  meshIds.clear();

  int shapeFlags = NvFlexMakeShapeFlags(eNvFlexShapeTriangleMesh, false);

  geometries    = new NvFlexVector<NvFlexCollisionGeometry>(library);
  geometryFlags = new NvFlexVector<int>(library);

  translations     = new NvFlexVector<glm::vec4>(library);
  prevTranslations = new NvFlexVector<glm::vec4>(library);
  rotations        = new NvFlexVector<glm::quat>(library);
  prevRotations    = new NvFlexVector<glm::quat>(library);

  geometries->map();
  geometryFlags->map();

  translations->map();
  prevTranslations->map();
  rotations->map();
  prevRotations->map();

  for (auto& elem : scene->root->children) {
    Mesh* mesh;
    if (mesh = dynamic_cast<Mesh*>(elem)) {
      size_t vertexCount = mesh->getVertices().size() / 3;
      size_t triangleCount = mesh->getIndices().size() / 3;
      
      NvFlexVector<float> positions(library);
      NvFlexVector<uint32_t> indices(library);

      positions.map();
      indices.map();

      positions.assign((float*)mesh->getVertices().data(), vertexCount * 3);
      indices.assign((uint32_t*)mesh->getIndices().data(), triangleCount * 3);
      
      positions.unmap();
      indices.unmap();

      glm::vec3 lower, upper;
      mesh->getBounds(lower, upper);

      meshIds.push_back(NvFlexCreateTriangleMesh(library));
      
      NvFlexUpdateTriangleMesh(
        library,
        meshIds.back(),
        positions.buffer,
        indices.buffer,
        vertexCount,
        triangleCount,
        (float*)&lower,
        (float*)&upper);

      NvFlexCollisionGeometry geometry;
      geometry.triMesh.mesh = meshIds.back();
      geometry.triMesh.scale[0] =  0.25f;
      geometry.triMesh.scale[1] =  0.25f;
      geometry.triMesh.scale[2] =  0.25f;
      geometries->push_back(geometry);
      geometryFlags->push_back(shapeFlags);
      
      translations->push_back(NO_TRANSLATION);
      prevTranslations->push_back(NO_TRANSLATION);
      rotations->push_back(NO_ROTATION);
      prevRotations->push_back(NO_ROTATION);
    }
  }

  geometries->unmap();
  geometryFlags->unmap();

  translations->unmap();
  prevTranslations->unmap();
  rotations->unmap();
  prevRotations->unmap();

  for (auto& elem : scene->root->children) {
    Fluid* fluid;
    if (fluid = dynamic_cast<Fluid*>(elem)) {
      NvFlexSetShapes(
        fluid->getSolver(),
        geometries->buffer,
        translations->buffer,
        rotations->buffer,
        prevTranslations->buffer,
        prevRotations->buffer,
        geometryFlags->buffer,
        geometries->size());
    }
  }
}

void FluidSimulation::update() {
  // Simulation step handled in the single Fluid instances
}

void FluidSimulation::cleanup() {
  delete geometries;
  delete geometryFlags;

  delete translations;
  delete prevTranslations;
  delete rotations;
  delete prevRotations;
}

void FluidSimulation::errorCallback(NvFlexErrorSeverity sev, const char* msg, const char* file, int line) {
  logger->error("Flex: {0} - {1}:{2}\n", msg, file, line);
}

const NvFlexParams defaultFluidParams = {
  3,                      // numIterations
  { 0, -9.8f, 0 },        // gravity
  0.05f,                  // radius
  0.1f,                   // solidRestDistance
  0.1f,                   // fluidRestDistance
  0.0f,                   // dynamicFriction
  0.0f,                   // staticFriction
  0.0f,                   // particleFriction
  0.0f,                   // restitution
  0.01f,                  // adhesion
  0.0f,                   // sleepThreshold
  FLT_MAX,                // maxSpeed
  FLT_MAX,                // maxAcceleration
  0.0f,                   // shockPropagation
  0.0f,                   // dissipation
  0.0f,                   // damping
  { 0, 0, 0 },            // wind
  0.0f,                   // drag
  0.0f,                   // lift
  false,                  // fluid
  0.01f,                  // cohesion
  0.0f,                   // surfaceTension
  0.0f,                   // viscosity
  40,                     // vorticityConfinement
  20,                     // anisotropyScale
  0.1f,                   // anisotropyMin
  2,                      // anisotropyMax
  1,                      // smoothing
  1,                      // solidPressure
  0,                      // freeSurfaceDrag
  1,                      // buoyancy
  FLT_MAX,                // diffuseThreshold
  1,                      // diffuseBuoyancy
  0.8f,                   // diffuseDrag
  16,                     // diffuseBallistic
  { 0, 0, 0 },            // diffuseSortAxis
  2,                      // diffuseLifetime
  0,                      // plasticThreshold
  0,                      // plasticCreep
  0.025f,                 // collisionDistance
  0.01f,                  // particleCollisionMargin
  0.01f,                  // shapeCollisionMargin
  { {  0,  1,  0,  2 },   // planes
    {  0,  0,  0,  0 },
    {  0,  0,  0,  0 },
    {  0,  0,  0,  0 },
    {  0,  0,  0,  0 },
    {  0,  0,  0,  0 },
    {  0,  0,  0,  0 },
    {  0,  0,  0,  0 } },
  1,                      // numPlanes
  eNvFlexRelaxationLocal, // relaxationMode
  1                       // relaxationFactor
};
