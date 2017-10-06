#include <fstream>
#include "scene.h"
#include "scene_element.h"
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

  root   = new SceneElement(this, ROOT_NODE(j), context);
  camera = new Camera(CAMERA_NODE(j));
  lights = parseLights(LIGHTS_NODE(j));
}

void Scene::update() {
  root->update();
}

void Scene::render() {
  root->render();
}

std::vector<Light*> Scene::parseLights(const ConfigNode& lightsNode) {
  std::vector<Light*> lights;
  for (size_t i = 0; i < lightsNode.size(); i++) {
    lights.push_back(new Light(lightsNode[i]));
  }
  return lights;
}
