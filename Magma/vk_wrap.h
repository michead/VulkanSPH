#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include "magma.h"

namespace VkWrap {
  inline VkResult createInstance(VkInstance& instance) {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = MAGMA_DISPLAY_NAME;
    appInfo.applicationVersion = MAGMA_VERSION;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
    
    return vkCreateInstance(&instanceInfo, nullptr, &instance);
  }
  inline VkResult getDevices(const VkInstance& instance, std::vector<VkPhysicalDevice>& deviceVec) {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    deviceVec.resize(deviceCount);
    return vkEnumeratePhysicalDevices(instance, &deviceCount, deviceVec.data());
  }
  inline void getQueueFamilyProperties(const VkPhysicalDevice device, std::vector<VkQueueFamilyProperties>& queueFamilyPropsVec) {
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    queueFamilyPropsVec.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilyPropsVec.data());
  }
  inline int32_t getQueueFamilyIndex(std::vector<VkQueueFamilyProperties>& queueFamilyPropsVec, const int32_t queueFamilyFlags) {
    for (uint32_t i = 0; i < queueFamilyPropsVec.size(); i++) {
      if (queueFamilyPropsVec[i].queueFlags & queueFamilyFlags) {
        return i;
      }
    }
    return -1;
  }
  inline VkResult createDevice(const VkPhysicalDevice& physicalDevice, VkDevice& device) {
    std::vector<VkQueueFamilyProperties> queueFamilyPropsVec;
    getQueueFamilyProperties(physicalDevice, queueFamilyPropsVec);
    int32_t queueFamilyIndex = getQueueFamilyIndex(queueFamilyPropsVec, VK_QUEUE_GRAPHICS_BIT);
    if (queueFamilyIndex < 0) {
      std::cout << "No suitable device for performing graphics operations has been found." << std::endl;
      return (VkResult)-1;
    }
    
    float queuePriorities[1] = { 0.f };
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;
    
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    return vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
  }
}