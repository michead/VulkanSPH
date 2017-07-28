#pragma once
#include "scene_element.h"
#include "magma_types.h"

struct MagmaContext;

class Mesh : public SceneElement {
public:
  Mesh(Scene* scene, const ConfigNode& elemObj, const MagmaContext* context);

  virtual void fromJSON(const ConfigNode& meshObj) override;

  std::vector<float>& getVertices();
  std::vector<uint32_t>& getIndices();

private:
  std::vector<float>    vertices;
  std::vector<uint32_t> indices;
};
