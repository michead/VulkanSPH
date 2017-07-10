#include <fstream>
#include <json.hpp>
#include "scene.h"

Scene::Scene(const char* filename, MVkContext* context) : context(context) {
  loadJSON(filename);
  camera->onViewportChange(context->viewport);
}

void Scene::loadJSON(const char* filename) {
  assert(filename);

  std::ifstream   i(filename);
  nlohmann::json  j;
  i >> j;

  camera = new Camera(CAMERA_NODE(j));
  graph  = new SceneGraph(this, ROOT_NODE(j), context);
}
