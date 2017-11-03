#include "pipeline.h"
#include "magma_context.h"
#include "subpass.h"
#include "gfx_utils.h"

Pipeline::Pipeline(const MagmaContext* context, Scene* scene, SceneElement* elem) : context(context), scene(scene), elem(elem) {
  init();
  postInit();
}

void Pipeline::init() {
  registerSubpasses();
  initPipelineState();
  initRenderPass();
  initVertexBuffers();
}

void Pipeline::postInit() {
  // TODO: Consider removing postInit hook
}

void Pipeline::initVertexBuffers() {
  GfxWrap::createBuffer(context->graphics->physicalDevice,
    context->graphics->device,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    fsQuadVertices.data(),
    4 * sizeof(MVkQuadVertexAttribute),
    &fsQuadBufferDesc);
}

void Pipeline::draw() {
  VkCommandBuffer cmdBuffer = context->graphics->getCurrentCmdBuffer();
  VkDeviceSize offset = 0;

  uint8_t i = 0;
  std::for_each(subpasses.begin(), subpasses.end(), [&](Subpass* subpass) {
    subpass->bind(cmdBuffer);

    if (!i) {
      vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBufferDesc.buffer, &offset);
      vkCmdDraw(cmdBuffer, elem->getVertexCount(), 1, 0, 0);
    } else {
      vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &fsQuadBufferDesc.buffer, &offset);
      vkCmdDraw(cmdBuffer, 4, 1, 0, 0);
    }

    i++;
  });
}

const MagmaContext* Pipeline::getContext() const {
  return context;
}

VkRenderPass Pipeline::getRenderPass() const {
  return renderPass;
}

std::vector<VkImageView> Pipeline::getColorAttachments() const {
  return colorAttachments;
}

VkImageView Pipeline::getDepthAttachment() const {
  return depthAttachment;
}

std::vector<Subpass*> Pipeline::getSubpasses() const {
  return subpasses;
}

const MVkPipelineParams BasePipeline = { 1 };
