#include "pipeline.h"
#include "gfx_context.h"

Pipeline::Pipeline(GfxContext* context, Camera* camera, SceneElement* elem) : context(context), camera(camera), elem(elem) {
  init();
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
  initUniformBuffers();
  updateDescriptorSets();

  initPipeline();
  context->setPipeline(this);

  initCommandBuffers();
}

void Pipeline::update() {
}

const MVkPipelineParams BasePipeline = { 1 };
