#pragma once
#include "camera.h"

#define CAMERA_NODE(j)    j["camera"]
#define ROOT_NODE(j)      j["root"]
#define CHILDREN_NODE(j)  j["children"]
#define TYPE_NODE(j)      j["type"]
#define VERTICES_NODE(j)  j["vertices"]
#define INDICES_NODE(j)   j["indices"]
#define OBJ_PATH_NODE(j)  j["source"]
#define HAS_CHILDREN(j)   j.find("children") != j.end()
#define IS_VERTEX_LIST(j) j.find("vertices") != j.end()
#define IS_OBJ_FILE(j)    j.find("source") != j.end()

struct SceneGraph;
struct MagmaContext;
class FluidSimulation;

struct Scene {
  Scene(const char* filename, const MagmaContext* context);

  Camera* camera;
  SceneGraph* graph;

  const MagmaContext* context;

  void loadJSON(const char* filename);
  
  void update();
  void render();
};