#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_beta.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <set>
#include <cstdint>

class Device {
public:
  Device(GLFWwindow* window, const char* applicationName, const char* engineName);
  ~Device();

  GLFWwindow* getWindow() const { return window; }
  VkDevice getDevice() const { return device; }
  VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
  VkQueue getGraphicsQueue() const { return graphicsQueue; }
  VkQueue getPresentQueue() const { return presentQueue; }
  VkSurfaceKHR getSurface() const { return surface; }
  VkSampleCountFlagBits getMsaaSamples() const { return msaaSamples; }

  // these are used later in the SwapChain
  uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;
	// uint32_t[] getQueueFamilyIndices() const {}

private:
  void createInstance(const char* applicationName, const char* engineName);
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  bool checkValidationLayerSupport();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  void printAvailableDeviceExtensions(VkPhysicalDevice device);

  GLFWwindow* window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  // multisampling
  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
  VkSampleCountFlagBits getMaxUsableSampleCount();

  #ifdef __APPLE__
  const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    // this is requested by the validation errors, but not available
    // on the Apple M1 chip
    // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
  };
  #else
  const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };
  #endif

  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  #ifdef NDEBUG
  const bool enableValidationLayers = false;
  #else
  const bool enableValidationLayers = true;
  #endif
};
