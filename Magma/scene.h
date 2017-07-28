#pragma once
#include "camera.h"

#define CAMERA_NODE(j)   j["camera"]
#define ROOT_NODE(j)     j["root"]
#define CHILDREN_NODE(j) j["children"]
#define TYPE_NODE(j)     j["type"]
#define HAS_CHILDREN(j)  j.find("children") != j.end()

struct SceneGraph;
struct MagmaContext;
class FluidSimulation;

struct Scene {
  Scene(const char* filename, const MagmaContext* context);

  Camera* camera;
  SceneGraph* graph;

  const MagmaContext* context;

  void loadJSON(const char* filename);
  
  void update();
  void render();
};