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

  // this will be cleaned up manually in the deconstructor
  VkSwapchainKHR swapChain;

  // these will be automatically cleaned up when the swap chain is destroyed
  std::vector<VkImage> swapChainImages;

  // these will be cleaned up manually in the deconstructor
  std::vector<VkImageView> swapChainImageViews;

  // this will be the image format (for example VK_FORMAT_B8G8R8A8_SRGB)
  // this will be needed later when we create a VkImage or VkImageView
  VkFormat swapChainImageFormat;

  // this is the size in pixels of the drawable area.
  // also needed later when we create a VkImage or VkImageView
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
