#include "fluid_simulation.h"
#include "scene.h"
#include "scene_graph.h"
#include "scene_element.h"
#include "mesh.h"

FluidSimulation::FluidSimulation() : library(NvFlexInit()), geometries(library), geometryFlags(library)  {

}

NvFlexLibrary* FluidSimulation::getLibrary() {
  return library;
}

void FluidSimulation::initCollision(Scene* scene) {
  this->scene = scene;

  meshIds.clear();

  for (auto& elem : scene->graph->root->children) {
    Mesh* mesh;
    if (mesh = dynamic_cast<Mesh*>(elem)) {
      meshIds.push_back(NvFlexCreateTriangleMesh(library));
      size_t vertexCount = mesh->getVertices().size() / 3;
      size_t triangleCount = mesh->getIndices().size() / 3;
      NvFlexVector<float> positions(library);
      NvFlexVector<uint32_t> indices(library);
      positions.assign((float*)mesh->getVertices().data(), vertexCount   * 3);
      indices.assign((uint32_t*)mesh->getIndices().data(), triangleCount * 3);
      positions.unmap();
      indices.unmap();
      NvFlexCollisionGeometry geometry;
      geometry.triMesh.mesh = meshIds.back();
      geometry.triMesh.scale[0] = 1;
      geometry.triMesh.scale[1] = 1;
      geometry.triMesh.scale[2] = 1;
      geometries.push_back(geometry);
      geometryFlags.push_back(NvFlexMakeShapeFlags(eNvFlexShapeTriangleMesh, false));
      NvFlexUpdateTriangleMesh(library, meshIds.back(), positions.buffer, indices.buffer, vertexCount, triangleCount, nullptr, nullptr);
    }
  }

  for (auto& elem : scene->graph->root->children) {
    Fluid* fluid;
    if (fluid = dynamic_cast<Fluid*>(elem)) {
      NvFlexSetShapes(fluid->getSolver(), geometries.buffer, nullptr, nullptr, NULL, NULL, geometryFlags.buffer, geometries.size());
    }
  }
}

void FluidSimulation::update() {

}
