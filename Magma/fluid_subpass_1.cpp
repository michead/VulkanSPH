#include "fluid_subpass_1.h"
#include "gfx_wrap.h"

void FluidSubpass1::init() {
  Subpass::init();
}

void FluidSubpass1::postInit() {
  Subpass::postInit();
}

void FluidSubpass1::initUniformBuffers() {
  uniformsFS.depthBuffer = gfxContext->depthBuffer;

  GfxWrap::createStagingBuffer(
    gfxContext->physicalDevice,
    gfxContext->device,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    &uniformsFS,
    sizeof(MVkFrag1),
    &stagingUniformBufferFSDesc,
    &uniformBufferFSDesc);
}

void FluidSubpass1::update() {
  Subpass::update();
}

void FluidSubpass1::updateDescriptorSets() {
  VkDescriptorImageInfo imageInfo = {};
  // TODO: Transition image layout to VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  imageInfo.imageView = gfxContext->depthBuffer.imageView;
  imageInfo.sampler = gfxContext->depthBuffer.sampler;

  GfxWrap::updateDescriptorSet(
    gfxContext->device,
    descriptorSet,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    0,
    uniformBufferFSDesc.bufferInfo,
    imageInfo);
}

void FluidSubpass1::updateUniformBuffers() {
  GfxWrap::updateBuffer(
    gfxContext->device,
    sizeof(MVkFrag1),
    &uniformsFS,
    stagingUniformBufferFSDesc.allocSize,
    stagingUniformBufferFSDesc.deviceMemory,
    &stagingUniformBufferFSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingFSBufferCmd,
    sizeof(MVkFrag1),
    gfxContext->graphicsQueue,
    stagingUniformBufferFSDesc.buffer,
    uniformBufferFSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    gfxContext->device,
    gfxContext->graphicsQueue,
    copyStagingFSBufferCmd);
}
