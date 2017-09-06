#include <fstream>
#include "scene.h"
#include "scene_graph.h"
#include "fluid.h"
#include "fluid_simulation.h"
#include "magma_types.h"
#include "magma_context.h"

Scene::Scene(const char* filename, const MagmaContext* context) : context(context) {
  loadJSON(filename);
  camera->onViewportChange(context->graphics->viewport);
}

void Scene::loadJSON(const char* filename) {
  assert(filename);

  std::ifstream i(filename);
  ConfigNode j;
  i >> j;

  camera = new Camera(CAMERA_NODE(j));
  lights = parseLights(LIGHTS_NODE(j));
  graph  = new SceneGraph(this, ROOT_NODE(j), context);
}

void Scene::update() {
  graph->root->update();
}

void Scene::render() {
  graph->root->render();
}

std::vector<Light> Scene::parseLights(const ConfigNode& lightsNode) {
  std::vector<Light> lights;
  for (size_t i = 0; i < lightsNode.size(); i++) {
    lights.push_back(Light(lightsNode[i]));
  }
  return lights;
}
