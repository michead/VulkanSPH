#include "pipeline.h"
#include "magma_context.h"
#include "subpass.h"

Pipeline::Pipeline(const MagmaContext* context, Scene* scene, SceneElement* elem) : context(context), scene(scene), elem(elem) {
  init();
  postInit();
}

void Pipeline::init() {
  initPipelineState();
  initRenderPass();
  initVertexBuffer();
}

void Pipeline::postInit() {
  // TODO: Consider removing postInit hook
}

VkRenderPass Pipeline::getRenderPass() const {
  return renderPass;
}

const MVkPipelineParams BasePipeline = { 1 };
