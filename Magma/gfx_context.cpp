#include "gfx_context.h"
#include "pipeline.h"
#include "gfx_wrap.h"
#include "gfx_utils.h"
#include "scene.h"
#include "magma.h"

bool GfxContext::bInit = false;

void GfxContext::init(const char* appName, uint32_t appVersion, HWND windowHandle, const Config* config) {
  initInstance(appName, appVersion);
  selectPhysicalDevice();
  initSurface(windowHandle);
  initDevice();
  initSwapchain(config->resolution);
  initDepthBuffer();
  GfxWrap::createSemaphore(device, imageAcquiredSemaphore);
  initCommandPools();
  initViewport();
  loadShaders();
  initDescriptorPool();
  initCommandBuffers();
  bInit = true;
}

void GfxContext::postInit() {
  assert(pipeline != nullptr);
  initFramebuffers();
}

void GfxContext::initInstance(const char* appName, uint32_t appVersion) {
  GfxWrap::createInstance(appName, appVersion, instance);
}

void GfxContext::initDevice() {
  GfxWrap::queryQueueFamilyIndices(physicalDevice, surface, &graphicsQueueFamilyIndex, &presentQueueFamilyIndex);
  GfxWrap::createDevice(physicalDevice, graphicsQueueFamilyIndex, presentQueueFamilyIndex, device, graphicsQueue, presentQueue);
}

void GfxContext::selectPhysicalDevice() {
  GfxWrap::queryDevices(instance, physicalDevices);
  if (physicalDevices.empty()) {
    logger->error("No Vulkan-enabled GPUs found.");
    exit(EXIT_FAILURE);
  }
  physicalDevice = physicalDevices.at(0);
  GfxWrap::printDeviceStats(physicalDevice);
}

void GfxContext::initCommandPools() {
  cmdPools.clear();

  for (size_t i = 0; i < swapchainImageCount; i++) {
    VkCommandPool cmdPool;
    VkCommandPoolCreateInfo info = {};
    info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext                   = nullptr;
    info.queueFamilyIndex        = graphicsQueueFamilyIndex;
    info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &cmdPool));
    VK_REGISTER(VkCommandPool, 1, &cmdPool);
    cmdPools.push_back(cmdPool);
  }
}

void GfxContext::initSurface(HWND hwnd) {
  GfxWrap::createSurface(instance, physicalDevice, hwnd,
    &graphicsQueueFamilyIndex, &presentQueueFamilyIndex,
    surface, surfaceCapabilities, format);
}

void GfxContext::initSwapchain(glm::ivec2 resolution) {
  swapchain.extent = {
    (uint32_t) resolution.x,
    (uint32_t) resolution.y
  };
  GfxWrap::createSwapchain(device,
    surface, surfaceCapabilities, format,
    graphicsQueueFamilyIndex, presentQueueFamilyIndex, 
    swapchain.handle, swapchain.extent, swapchain.images, swapchain.imageViews);
  currentImageIndex = 0;

  swapchainImageCount = swapchain.imageViews.size();
  drawFences.resize(swapchainImageCount);
  for (size_t i = 0; i < swapchainImageCount; i++) {
    GfxWrap::createFence(device, drawFences[i]);
  }
}

void GfxContext::initDescriptorPool() {
  VkDescriptorPoolSize descriptorPoolSize = VK_DESCRIPTOR_POOL_SIZE;
  GfxWrap::createDescriptorPool(device, descriptorPoolSize, descriptorPool);
}

void GfxContext::initDepthBuffer() {
  VkDeviceMemory deviceMemory;
  GfxWrap::createDepthBuffer(
    physicalDevice,
    device,
    swapchain.extent,
    VK_NUM_IMAGE_SAMPLES,
    depthBuffer.image,
    deviceMemory,
    depthBuffer.imageView,
    depthBuffer.sampler);
}

void GfxContext::loadShaders() {
  HANDLE hFind;
  WIN32_FIND_DATA data;

  std::wstring wDir(SHADER_PATH);
  
  hFind = FindFirstFile((wDir + L"*").c_str(), &data);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      char cGlsl[128];
      sprintf_s(cGlsl, "%ws", data.cFileName);
      if (GfxUtils::isGLSLFilename(cGlsl)) {
        logger->info("Compiling shader {0} ...", cGlsl);
        
        std::string sGlsl = std::string(cGlsl);
        std::string shaderName  = sGlsl.substr(0, sGlsl.size() - 5);
        std::string shaderStage = sGlsl.substr(   sGlsl.size() - 4);
        std::string sSpv = shaderName + "." + shaderStage + ".spv";

        char cDir[128];
        sprintf_s(cDir, "%ws", wDir.c_str());

        char cGlslAbs[256];
        strcpy_s(cGlslAbs, cDir);
        strcat_s(cGlslAbs, cGlsl);

        char cSpvAbs[256];
        strcpy_s(cSpvAbs, cDir);
        strcat_s(cSpvAbs, sSpv.c_str());

        char command[512];
        sprintf_s(command, VK_SDK_BIN"glslangValidator \"%s\" -V -o \"%s\"", cGlslAbs, cSpvAbs);
        
        int ret = system(command);
        if (ret != 0) {
          logger->error("Shader compilation failed.");
          getchar();
          exit(EXIT_FAILURE);
        }
        
        if (shaderMap.find(shaderName) == shaderMap.end()) {
          shaderMap[shaderName] = std::map<std::string, std::vector<uint32_t>>();
          if (shaderMap[shaderName].find(shaderStage) == shaderMap[shaderName].end()) {
            shaderMap[shaderName][shaderStage] = std::vector<uint32_t>();
          } else {
            shaderMap[shaderName][shaderStage].clear();
          }
        }

        std::string sSpvAbs(cSpvAbs);
        std::ifstream in(sSpvAbs, std::ios::ate | std::ios::binary);

        if (!in.is_open()) {
          logger->error("Shader file cannot be opened.");
          getchar();
          exit(EXIT_FAILURE);
        }

        size_t fileSize = (size_t) in.tellg();
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

        in.seekg(0);
        in.read((char*)buffer.data(), fileSize);
        in.close();

        shaderMap[shaderName][shaderStage] = buffer;
      }
    } while (FindNextFile(hFind, &data));
    
    FindClose(hFind);
  }

  logger->info("Shader compilation complete.");
}

void GfxContext::initViewport() {
  viewport = GfxUtils::viewport( swapchain.extent.width, swapchain.extent.height );
  scissor  = GfxUtils::scissor({ swapchain.extent.width, swapchain.extent.height });
}

void GfxContext::initFramebuffers() {
  GfxWrap::createFramebuffers(
    device,
    pipeline->getRenderPass(),
    swapchain.imageViews,
    depthBuffer.imageView,
    swapchain.extent.width,
    swapchain.extent.height,
    framebuffers);
}

void GfxContext::initCommandBuffers() {
  drawCmds.clear();
  for (size_t i = 0; i < swapchainImageCount; i++) {
    VkCommandBuffer drawCmd;
    GfxWrap::createCommandBuffers(device, cmdPools[i], 1, &drawCmd);
    drawCmds.push_back(drawCmd);
  }
}

void GfxContext::setPipeline(Pipeline* pipeline) {
  this->pipeline = pipeline;
}

Pipeline* GfxContext::getPipeline() const {
  return pipeline;
}

VkRenderPass GfxContext::getRenderPass() const {
  return pipeline->getRenderPass();
}

VkPipelineCache GfxContext::getPipelineCache() const {
  return VK_NULL_HANDLE;
}

void GfxContext::setScene(Scene* scene) {
  this->scene = scene;
}

Scene* GfxContext::getScene() const {
  return scene;
}

std::vector<VkCommandBuffer> GfxContext::getCmdBuffers() const {
  return drawCmds;
}

VkCommandBuffer GfxContext::getCurrentCmdBuffer() const {
  return drawCmds[currentImageIndex];
}

std::vector<VkCommandPool> GfxContext::getCmdPools() const {
  return cmdPools;
}

VkCommandPool GfxContext::getCurrentCmdPool() const {
  return cmdPools[currentImageIndex];
}

std::vector<VkFramebuffer> GfxContext::getFramebuffers() const {
  return framebuffers;
}

VkFramebuffer GfxContext::getCurrentFramebuffer() const {
  return framebuffers[currentImageIndex];
}
