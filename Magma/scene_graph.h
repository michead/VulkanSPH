#pragma once

#include "scene_element.h"
#include "fluid.h"

#define is(a, b) (!strcmp((a).c_str(), (b)))

struct SceneGraph {
  SceneGraph(Scene* scene, const nlohmann::json& jsonObj, MVkContext* context) : scene(scene), context(context) { fromJSON(jsonObj); }

  Scene* scene;
  SceneElement* root;

  MVkContext* context;

  void fromJSON(const nlohmann::json& jsonObj) {
    std::string elemType = jsonObj["type"];
    if (is(elemType, SCENE_ELEM_PARTICLE_GRID)) {
      root = new Fluid(scene, jsonObj, context);
    } else {
      assert(false && "Unsupported scene element type.");
    }
  }
};
