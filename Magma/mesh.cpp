#include "mesh.h"
#include "magma_context.h"

Mesh::Mesh(Scene* scene, const ConfigNode& elemObj, const MagmaContext* context) : SceneElement(scene, elemObj, context) {}

void Mesh::fromJSON(const ConfigNode& meshObj) {
  const nlohmann::json jVertices = meshObj["vertices"];
  const nlohmann::json jIndices  = meshObj["indices"];

  for (const auto& n : jVertices) {
    vertices.push_back((float)n);
  }

  for (const auto& n : jIndices) {
    indices.push_back((uint32_t)n);
  }

  assert(vertices.size() % 3 == 0);
  assert(indices.size()  % 3 == 0);
}

std::vector<float>& Mesh::getVertices() {
  return vertices;
}

std::vector<uint32_t>& Mesh::getIndices() {
  return indices;
}
