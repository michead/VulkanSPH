#include "pipeline.h"
#include "gfx_context.h"

Pipeline::Pipeline(GfxContext* context, Scene* scene, SceneElement* elem) : context(context), scene(scene), elem(elem) {
  init();
  postInit();
}

VkCommandBuffer Pipeline::getDrawCmdBuffer() const {
  return drawCmds[context->currentImageIndex];
}

void Pipeline::init() {
  initPipelineState();
  initRenderPass();
  initFramebuffers();
  initStages();
  initVertexBuffer();
  initPipelineLayout();
  initPipelineCache();
}

void Pipeline::postInit() {
  initUniformBuffers();
  updateDescriptorSets();
  initCommandBuffers();
}

void Pipeline::update() {
}

const MVkPipelineParams BasePipeline = { 1 };
