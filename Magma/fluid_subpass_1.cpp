#include "fluid_subpass_1.h"
#include "gfx_wrap.h"

void FluidSubpass1::init() {
  Subpass::init();
}

void FluidSubpass1::postInit() {
  Subpass::postInit();
}

void FluidSubpass1::initUniformBuffers() {
  uniformsFS.depthBuffer = context->graphics->depthBuffer;

  GfxWrap::createStagingBuffer(
    context->graphics->physicalDevice,
    context->graphics->device,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    &uniformsFS,
    sizeof(MVkFrag1),
    &stagingUniformBufferFSDesc,
    &uniformBufferFSDesc);
}

void FluidSubpass1::update() {
  Subpass::update();
}

void FluidSubpass1::updateUniformBuffers() {
  GfxWrap::updateBuffer(
    device,
    sizeof(MVkFrag1),
    &uniformsFS,
    stagingUniformBufferFSDesc.allocSize,
    stagingUniformBufferFSDesc.deviceMemory,
    &stagingUniformBufferFSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingFSBufferCmd,
    sizeof(MVkFrag1),
    context->graphics->graphicsQueue,
    stagingUniformBufferFSDesc.buffer,
    uniformBufferFSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    context->graphics->device,
    context->graphics->graphicsQueue,
    copyStagingFSBufferCmd);
}
