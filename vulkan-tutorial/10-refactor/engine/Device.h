#pragma once

#include <vulkan/vulkan.h>
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

  GLFWwindow* window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

  const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
};
