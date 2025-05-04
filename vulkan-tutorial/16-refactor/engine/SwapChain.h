#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"

class SwapChain {
public:
  SwapChain(Device& device);
  ~SwapChain();

  VkSwapchainKHR getSwapChain() const { return swapChain; }
  VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() const { return swapChainExtent; }
  const std::vector<VkImageView>& getSwapChainImageViews() const {
    return swapChainImageViews;
  }

  void recreateSwapChain();

private:
  void deallocAll();
  void createSwapChain();
  void createImageViews();

  Device& device;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats
  );
  VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes
  );
  VkExtent2D chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities
  );
};
