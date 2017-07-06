#pragma once

#include "camera.h"
#include "scene_graph.h"
#include "fluid.h"
#include "mvk_context.h"

#define CAMERA_NODE(j) j["camera"]
#define ROOT_NODE(j)   j["root"]

struct Scene {
  Scene(const char* filename, MVkContext* context);

  Camera* camera;
  SceneGraph* graph;

  MVkContext* context;

  void loadJSON(const char* filename);
  
  void update() { graph->root->update(); }
  void render() { graph->root->render(); }
};