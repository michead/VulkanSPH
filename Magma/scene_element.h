#pragma once

#include <vulkan\vulkan.hpp>
#include "gfx_wrap.h"
#include "pipeline.h"
#include "magma_types.h"

#define SCENE_ELEM_PARTICLE_GRID "PARTICLE_GRID"
#define SCENE_ELEM_TRIANGLE_MESH "TRIANGLE_MESH"

struct Scene;
struct MagmaContext;

struct SceneElement {
  SceneElement(Scene* scene, const ConfigNode& elemObj, const MagmaContext* context);

  virtual void fromJSON(const ConfigNode& elemObj);
  virtual void update();
  virtual void render();
  virtual size_t getVertexCount() const { return 0; }

  SceneElement* parseSceneElem(const ConfigNode& jsonObj);
  glm::mat4 getModelMatrix() const;

  std::vector<SceneElement*> children;

protected:
  Scene* scene;

  const MagmaContext* context;
  Pipeline* pipeline;

  glm::mat4 model;
};