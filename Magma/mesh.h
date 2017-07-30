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
  void getBounds(glm::vec3& lower, glm::vec3& upper);

private:
  void loadVertices(const ConfigNode& elemObj);
  void loadOBJFile(const ConfigNode& elemObj);

  std::vector<float>    vertices;
  std::vector<uint32_t> indices;
};
