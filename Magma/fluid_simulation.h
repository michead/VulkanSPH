#pragma once
#include <NvFlexExt.h>
#include <glm\gtc\quaternion.hpp>
#include "scene.h"

#define NO_TRANSLATION glm::vec4(0)
#define NO_ROTATION    glm::quat()

class FluidSimulation {
public:
  FluidSimulation();
  ~FluidSimulation();

  void initCollision(Scene* scene);
  void update();

  NvFlexLibrary* getLibrary();

private:
  void cleanup();

  static void errorCallback(NvFlexErrorSeverity sev, const char* msg, const char* file, int line);

  NvFlexLibrary* library;
  Scene* scene;
  std::vector<NvFlexTriangleMeshId> meshIds;
  NvFlexVector<NvFlexCollisionGeometry>* geometries;
  NvFlexVector<int>* geometryFlags;
  NvFlexVector<glm::vec4>* translations;
  NvFlexVector<glm::vec4>* prevTranslations;
  NvFlexVector<glm::quat>* rotations;
  NvFlexVector<glm::quat>* prevRotations;
};

extern const NvFlexParams defaultFluidParams;
