#pragma once

#include <vulkan\vulkan.hpp>
#include <json.hpp>
#include "mvk_wrap.h"
#include "mvk_context.h"
#include "mvk_pipeline.h"

#define SCENE_ELEM_PARTICLE_GRID "PARTICLE_GRID"

struct Scene;

struct SceneElement {
  SceneElement(Scene* scene, const nlohmann::json& jsonObj, MVkContext* context) : scene(scene), context(context) {}

  virtual void fromJSON(const nlohmann::json& jsonObj) = 0;
  virtual void update() { pipeline->update(); }
  virtual void render() {
    VkCommandBuffer drawCmd = pipeline->getDrawCmdBuffer();
    VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    MVkWrap::submitCommandBuffer(
      context->graphicsQueue,
      1,
      &drawCmd,
      1,
      &context->imageAcquiredSemaphore,
      &stageFlags,
      context->drawFences[context->currentSwapchainImageIndex]);

    for (const auto& child : children) {
      child->render();
    }
  }

  std::vector<SceneElement*> children;

protected:
  SceneElement() {}

  Scene* scene;

  MVkContext*  context;
  MVkPipeline* pipeline;
};