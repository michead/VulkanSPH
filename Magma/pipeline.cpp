#include "pipeline.h"
#include "gfx_context.h"
#include "subpass.h"

Pipeline::Pipeline(GfxContext* context, Scene* scene, SceneElement* elem) : context(context), scene(scene), elem(elem) {
  init();
  postInit();
}

void Pipeline::init() {
  initPipelineState();
  initRenderPass();
  initFramebuffers();
  initVertexBuffer();
}

void Pipeline::postInit() {
  initCommandBuffers();
}

VkCommandBuffer Pipeline::getDrawCmdBuffer() const {
  return drawCmds[context->currentImageIndex];
}

VkRenderPass Pipeline::getRenderPass() const {
  return renderPass;
}

const MVkPipelineParams BasePipeline = { 1 };
