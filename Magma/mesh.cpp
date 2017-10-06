#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <fstream>
#include "mesh.h"
#include "magma_context.h"
#include "logger.h"

Mesh::Mesh(Scene* scene, const ConfigNode& elemObj, const MagmaContext* context) : SceneElement(scene, elemObj, context) {
  fromJSON(elemObj);
}

void Mesh::fromJSON(const ConfigNode& meshObj) {
  if (IS_VERTEX_LIST(meshObj)) {
    loadVertices(meshObj);
  } else if (IS_OBJ_FILE(meshObj)) {
    loadOBJFile(meshObj);
  }
}

std::vector<float>& Mesh::getVertices() {
  return vertices;
}

std::vector<uint32_t>& Mesh::getIndices() {
  return indices;
}

void Mesh::getBounds(glm::vec3& lower, glm::vec3& upper) {
  glm::vec3 _lower(FLT_MAX);
  glm::vec3 _upper(-FLT_MAX);

  for (uint32_t i = 0; i < vertices.size(); i += 3) {
    const glm::vec3& pos = { vertices[i], vertices[i + 1], vertices[i + 2] };

    _lower = pos.length() < _lower.length() ? pos : _lower;
    _upper = pos.length() > _lower.length() ? pos : _upper;
  }

  lower = glm::vec3(_lower);
  upper = glm::vec3(_upper);
}

void Mesh::loadVertices(const ConfigNode& meshObj) {
  const nlohmann::json jVertices = VERTICES_NODE(meshObj);
  const nlohmann::json jIndices = INDICES_NODE(meshObj);

  for (const auto& n : jVertices) {
    vertices.push_back((float) n);
  }

  for (const auto& n : jIndices) {
    indices.push_back((uint32_t) n);
  }

  assert(vertices.size() % 3 == 0);
  assert(indices.size() % 3 == 0);
}

void Mesh::loadOBJFile(const ConfigNode& elemObj) {
  std::string path = OBJ_PATH_NODE(elemObj);
  tinyobj::attrib_t                attrib;
  std::vector<tinyobj::shape_t>    shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str());

  if (!err.empty()) {
    logger->warn(err);
  }

  if (!ret) {
    logger->error(".obj file {0} could not be parsed.", path.c_str());
    exit(EXIT_FAILURE);
  }

  vertices.clear();
  indices.clear();

  for each (auto& vert in attrib.vertices) {
    vertices.push_back(vert);
  }

  for each (auto& index in shapes[0].mesh.indices) {
    indices.push_back(index.vertex_index);
  }
}
