#pragma once
#include "scene_element.h"
#include "fluid.h"
#include "fluid_simulation.h"
#include "magma_types.h"

#define is(a, b) (!strcmp((a).c_str(), (b)))

struct MagmaContext;

struct SceneGraph {
  SceneGraph(Scene* scene, ConfigNode rootNode, const MagmaContext* context)
    : scene(scene), context(context){ fromJSON(rootNode); }

  Scene* scene;
  SceneElement* root;

  const MagmaContext* context;

  void fromJSON(const nlohmann::json& rootNode) {
    root = new SceneElement(scene, rootNode, context);
  }
};
