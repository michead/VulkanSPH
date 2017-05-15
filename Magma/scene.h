#pragma once

#include "camera.h"
#include "scene_graph.h"

struct Scene {
  Camera camera;
  SceneGraph graph;
  
  void draw() {
    graph.root->draw();
  }
};