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
  std::ifstream file(path.c_str());

  if (!file) {
    logger->error("OBJ file at {0} cannot be found.", path.c_str());
    exit(EXIT_FAILURE);
  }

  char buffer[1024];

  while (file) {
    file >> buffer;

    if (buffer[0] == 'v') {
      float x, y, z;
      file >> x >> y >> z;

      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    } else if (buffer[0] == 'f') {
      unsigned int x, y, z;
      file >> x >> y >> z;

      indices.push_back(x - 1);
      indices.push_back(y - 1);
      indices.push_back(z - 1);
    }
  }

  assert(vertices.size() % 3 == 0);
  assert(indices.size() % 3 == 0);
}
