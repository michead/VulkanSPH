#include "fluid_pipeline.h"
#include "fluid_subpass_0.h"
#include "fluid_subpass_1.h"
#include "fluid.h"
#include "magma_context.h"
#include "gfx_wrap.h"
#include "gfx_structs.h"
#include "gfx_utils.h"
#include "subpass.h"
#include "scene.h"

FluidPipeline::FluidPipeline(const MagmaContext* context, Scene* scene, Fluid* elem) : Pipeline(context, scene, elem) {
  init();
}

void FluidPipeline::init() {
  Pipeline::init();
}

void FluidPipeline::registerSubpasses() {
  subpasses.clear();
  subpasses.push_back(new FluidSubpass0(this, 0, elem));
  subpasses.push_back(new FluidSubpass1(this, 1, elem));

  std::for_each(subpasses.begin(), subpasses.end(), [](Subpass* subpass) {
    subpass->init();
  });
}

void FluidPipeline::postInit() {
  std::for_each(subpasses.begin(), subpasses.end(), [] (Subpass* subpass) {
    subpass->postInit();
  });
}

void FluidPipeline::initRenderPass() {
  std::vector<VkSubpassDescription> subpassDescriptions;
  std::vector<VkSubpassDependency> subpassDependencies;

  std::for_each(subpasses.begin(), subpasses.end(), [&](Subpass* subpass) {
    subpassDescriptions.push_back(subpass->getSubpassDescription());
    subpassDependencies.push_back(subpass->getSubpassDependency());
  });

  GfxWrap::createRenderPass(
    context->graphics->device,
    attachments,
    subpassDescriptions,
    subpassDependencies,
    renderPass);
}

void FluidPipeline::update() {
  for each(const auto& subpass in subpasses) {
    subpass->update();
  }
}
