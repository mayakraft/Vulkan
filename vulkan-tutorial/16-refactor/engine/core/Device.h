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
  VkCommandPool getCommandPool() const { return commandPool; }
  VkSurfaceKHR getSurface() const { return surface; }
  VkSampleCountFlagBits getMsaaSamples() const { return msaaSamples; }

  // these are only used by the SwapChain
  uint32_t getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }
  uint32_t getPresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }

private:
  void createInstance(const char* applicationName, const char* engineName);
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();

  bool checkValidationLayerSupport();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  void printAvailableDeviceExtensions(VkPhysicalDevice device);

  GLFWwindow* window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;

  // the physical hardware device (GPU) we are initializing
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  // the logical device
  // this will be referenced by nearly every Vulkan function call
  VkDevice device;

  // device queues will be automatically cleaned up when the logical device is freed
  // queues are used to submit commands to the GPU.

  // graphics queue is used to submit rendering commands
  VkQueue graphicsQueue;

  // presentation queue is used to post the renderings to the surface
  VkQueue presentQueue;

  // the command pool is used to create command buffers, copying buffers,
  // creating images, creating mipmaps, various basic memory operations
  VkCommandPool commandPool;

  // these are used to create the queues,
  // and these indices themselves are needed by the swap chain
  uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;

  // multisample anti-aliasing
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
