#include "mvk_context.h"
#include "mvk_wrap.h"
#include "mvk_utils.h"
#include "magma.h"

bool MVkContext::bInit = false;

void MVkContext::init(const char* appName, uint32_t appVersion, HWND windowHandle) {
  initInstance(appName, appVersion);
  selectPhysicalDevice();
  initSurface(windowHandle);
  initDevice();
  initSwapchain();
  initDepthBuffer();
  MVkWrap::createSemaphore(device, imageAcquiredSemaphore);
  MVkWrap::createFence(device, drawFence);
  initCommandPool();
  initCommandBuffer();
  initViewport();
  loadShaders();
  initDescriptorPool();
  bInit = true;
}

void MVkContext::setValidationLayers() {
  validationLayers.clear();
#if defined(_DEBUG)
  validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif
}

void MVkContext::initInstance(const char* appName, uint32_t appVersion) {
  MVkWrap::createInstance(appName, appVersion, instance);
}

void MVkContext::initDevice() {
  MVkWrap::queryQueueFamilyIndices(physicalDevice, surface, &graphicsQueueFamilyIndex, &presentQueueFamilyIndex);
  MVkWrap::createDevice(physicalDevice, graphicsQueueFamilyIndex, presentQueueFamilyIndex, device, graphicsQueue, presentQueue);
}
void MVkContext::selectPhysicalDevice() {
  MVkWrap::queryDevices(instance, physicalDevices);
  if (physicalDevices.empty()) {
    logger->error("No Vulkan-enabled GPUs found.");
    exit(EXIT_FAILURE);
  }
  physicalDevice = physicalDevices.at(0);
  MVkWrap::printDeviceStats(physicalDevice);
}

void MVkContext::initCommandPool() {
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;
  info.queueFamilyIndex = graphicsQueueFamilyIndex;
  info.flags = 0;
  VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &commandPool));
  VK_REGISTER(VkCommandPool, 1, &commandPool);
}

void MVkContext::initSurface(HWND hwnd) {
  MVkWrap::createSurface(instance, physicalDevice, hwnd,
    &graphicsQueueFamilyIndex, &presentQueueFamilyIndex,
    surface, surfaceCapabilities, format);
}

void MVkContext::initSwapchain() {
  MVkWrap::createSwapchain(device,
    surface, surfaceCapabilities, format,
    graphicsQueueFamilyIndex, presentQueueFamilyIndex, 
    swapchain.handle, swapchain.extent, swapchain.images, swapchain.imageViews);
  currentSwapchainImageIndex = 0;
}

void MVkContext::initDescriptorPool() {
  VkDescriptorPoolSize descriptorPoolSize = VK_DESCRIPTOR_POOL_SIZE;
  MVkWrap::createDescriptorPool(device, descriptorPoolSize, descriptorPool);
}


void MVkContext::initCommandBuffer() {
  MVkWrap::createCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void MVkContext::initDepthBuffer() {
  VkDeviceMemory deviceMemory;
  MVkWrap::createDepthBuffer(
    physicalDevice,
    device,
    swapchain.extent,
    VK_NUM_IMAGE_SAMPLES,
    depthBuffer.image,
    deviceMemory,
    depthBuffer.imageView);
}

void MVkContext::loadShaders() {
  HANDLE hFind;
  WIN32_FIND_DATA data;

  std::wstring wDir(SHADER_PATH);
  wDir += L"*";
  
  hFind = FindFirstFile(wDir.c_str(), &data);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      char cGlsl[128];
      sprintf_s(cGlsl, "%ws", data.cFileName);
      if (MVkUtils::isGLSLFilename(cGlsl)) {
        logger->info("Compiling shader {0} ...", cGlsl);
        
        std::string sGlsl = std::string(cGlsl);
        std::string shaderName  = sGlsl.substr(0, sGlsl.size() - 5);
        std::string shaderStage = sGlsl.substr(   sGlsl.size() - 4);

        char command[128];
        std::string sSpv = shaderName + "." + shaderStage + ".spv";
        sprintf_s(command, VK_SDK_BIN"glslangValidator %s -V -o %s", cGlsl, sSpv.c_str());
        
        int ret = system(command);
        if (ret != 0) {
          logger->error("Shader compilation failed.");
          getchar();
          exit(EXIT_FAILURE);
        }
        
        if (shaderMap.find(shaderName) == shaderMap.end()) {
          shaderMap[shaderName] = std::map<std::string, std::vector<char>>();
          if (shaderMap[shaderName].find(shaderStage) == shaderMap[shaderName].end()) {
            shaderMap[shaderName][shaderStage] = std::vector<char>();
          } else {
            shaderMap[shaderName][shaderStage].clear();
          }
        }

        std::ifstream in(sSpv, std::ios::ate | std::ios::binary);

        if (!in.is_open()) {
          logger->error("Shader file cannot be opened.");
          getchar();
          exit(EXIT_FAILURE);
        }

        size_t fileSize = (size_t) in.tellg();
        std::vector<char> buffer(fileSize);

        in.seekg(0);
        in.read(buffer.data(), fileSize);
        in.close();

        shaderMap[shaderName][shaderStage] = buffer;
      }
    } while (FindNextFile(hFind, &data));
    
    FindClose(hFind);
  }

  logger->info("Shader compilation complete.");
}

void MVkContext::initViewport() {
  viewport = MVkUtils::viewport( swapchain.extent.width, swapchain.extent.height );
  scissor  = MVkUtils::scissor({ swapchain.extent.width, swapchain.extent.height });
}
