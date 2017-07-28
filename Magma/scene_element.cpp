#include "scene_element.h"
#include "scene.h"
#include "mesh.h"
#include "fluid.h"
#include <string>
#include <vector>
#include "magma_context.h"
#include "logger.h"

SceneElement::SceneElement(Scene* scene, const ConfigNode& elemObj, const MagmaContext* context) : scene(scene), context(context) {
  fromJSON(elemObj);

  pipeline = nullptr;
}

void SceneElement::fromJSON(const ConfigNode& elemObj) {
  if (HAS_CHILDREN(elemObj)) {
    for (size_t i = 0; i < CHILDREN_NODE(elemObj).size(); i++) {
      children.push_back(parseSceneElem(CHILDREN_NODE(elemObj)[i]));
    }
  }
}

void SceneElement::update() {
  if (pipeline) {
    pipeline->update();
  }
}

void SceneElement::render() {
  if (pipeline) {
    VkCommandBuffer drawCmd = pipeline->getDrawCmdBuffer();
    VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    MVkWrap::submitCommandBuffer(
      context->graphics->graphicsQueue,
      1,
      &drawCmd,
      1,
      &context->graphics->imageAcquiredSemaphore,
      &stageFlags,
      context->graphics->drawFences[context->graphics->currentImageIndex]);
  }

  for (const auto& child : children) {
    child->render();
  }
}

glm::mat4 SceneElement::getModelMatrix() const {
  return model;
}

SceneElement* SceneElement::parseSceneElem(const ConfigNode& elemObj) {
  SceneElement* sceneElem;
  std::string elemType = TYPE_NODE(elemObj).get<std::string>();
  if (elemType == SCENE_ELEM_PARTICLE_GRID) {
    sceneElem = new Fluid(scene, elemObj, context);
  } else if (elemType == SCENE_ELEM_TRIANGLE_MESH) {
    sceneElem = new Mesh(scene, elemObj, context);
  } else {
    logger->error("Unknown scene element type: {0}", elemType.c_str());
  }
  return sceneElem;
}
