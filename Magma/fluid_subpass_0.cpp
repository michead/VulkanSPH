#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "fluid_subpass_0.h"
#include "magma_context.h"
#include "fluid.h"
#include "scene.h"
#include "gfx_utils.h"

void FluidSubpass0::init() {
  Subpass::init();
}

void FluidSubpass0::postInit() {
  Subpass::postInit();
}

void FluidSubpass0::initUniformBuffers() {
  uniformsVS.view = scene->camera->getViewMatrix();
  uniformsVS.proj = scene->camera->getProjectionMatrix();
  uniformsVS.particleSize = to_fluid(elem)->radius * 1000;

  GfxWrap::createStagingBuffer(
    context->graphics->physicalDevice,
    context->graphics->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsVS,
    sizeof(MVkVert0),
    &stagingUniformBufferVSDesc,
    &uniformBufferVSDesc);

  uniformsFS.particleSize = uniformsVS.particleSize;
  uniformsFS.ambientColor = glm::vec4(0.5f, 0.5f, 0.5, 1.f);
  uniformsFS.fluidDiffuse = glm::vec4(0.0f, 0.0f, 1.f, 1.f);
  uniformsFS.lightCount = scene->lights.size();
  memcpy(uniformsFS.lights, scene->lights.data(), sizeof(Light) * uniformsFS.lightCount);
  uniformsFS.proj = scene->camera->getProjectionMatrix();
  uniformsFS.invProj = glm::inverse(scene->camera->getProjectionMatrix());
  uniformsFS.viewport = {
    viewport.x,
    viewport.y,
    viewport.width,
    viewport.height };

  GfxWrap::createStagingBuffer(
    context->graphics->physicalDevice,
    context->graphics->device,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    &uniformsFS,
    sizeof(MVkFrag0),
    &stagingUniformBufferFSDesc,
    &uniformBufferFSDesc);
}

void FluidSubpass0::update() {
  Subpass::update();
}

void FluidSubpass0::updateUniformBuffers() {
  uniformsVS.view = scene->camera->getViewMatrix();
  uniformsVS.proj = scene->camera->getProjectionMatrix();

  GfxWrap::updateBuffer(
    device,
    sizeof(MVkVert0),
    &uniformsVS,
    stagingUniformBufferVSDesc.allocSize,
    stagingUniformBufferVSDesc.deviceMemory,
    &stagingUniformBufferVSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingVSBufferCmd,
    sizeof(MVkVert0),
    context->graphics->graphicsQueue,
    stagingUniformBufferVSDesc.buffer,
    uniformBufferVSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    context->graphics->device,
    context->graphics->graphicsQueue,
    copyStagingVSBufferCmd);

  std::vector<Light> lights;
  std::transform(scene->lights.begin(), scene->lights.end(), std::back_inserter(lights), [](Light* light) {
    return *light;
  });
  memcpy(&uniformsFS, lights.data(), sizeof(Light) * lights.size());
  uniformsFS.lightCount = lights.size();
  uniformsFS.proj = uniformsVS.proj;
  uniformsFS.invProj = glm::inverse(uniformsVS.proj);
  uniformsFS.viewport = {
    viewport.x,
    viewport.y,
    viewport.width,
    viewport.height };

  GfxWrap::updateBuffer(
    device,
    sizeof(MVkFrag0),
    &uniformsFS,
    stagingUniformBufferFSDesc.allocSize,
    stagingUniformBufferFSDesc.deviceMemory,
    &stagingUniformBufferFSDesc.mappedMemory);

  GfxWrap::registerCopyCmd(
    copyStagingFSBufferCmd,
    sizeof(MVkFrag0),
    context->graphics->graphicsQueue,
    stagingUniformBufferFSDesc.buffer,
    uniformBufferFSDesc.buffer);

  GfxWrap::submitCmdBuffer(
    context->graphics->device,
    context->graphics->graphicsQueue,
    copyStagingFSBufferCmd);
}
