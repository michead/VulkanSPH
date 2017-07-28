#pragma once
#include <NvFlexExt.h>
#include "scene.h"

class FluidSimulation {
public:
  FluidSimulation();

  void initCollision(Scene* scene);
  void update();

  NvFlexLibrary* getLibrary();

private:
  NvFlexLibrary* library;
  Scene* scene;
  std::vector<NvFlexTriangleMeshId> meshIds;
  NvFlexVector<NvFlexCollisionGeometry> geometries;
  NvFlexVector<int> geometryFlags;
};
