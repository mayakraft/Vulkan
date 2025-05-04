#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Image.h"
#include "ImageView.h"

class SwapChainResources {
private:
  VkDevice device;
  VkPhysicalDevice physicalDevice;
  VkExtent2D swapChainExtent;
  VkSampleCountFlagBits msaaSamples;
  VkFormat colorFormat;
  VkFormat depthFormat;
  VkRenderPass renderPass;
  const std::vector<VkImageView>& swapChainImageViews;

  // color image
  Image colorImage;
  ImageView colorImageView;

  // depth
  Image depthImage;
  ImageView depthImageView;

  // framebuffers
  std::vector<VkFramebuffer> swapChainFramebuffers;

public:
  SwapChainResources(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkExtent2D swapChainExtent,
    VkSampleCountFlagBits msaaSamples,
    VkFormat colorFormat,
    VkFormat depthFormat,
    const std::vector<VkImageView>& swapChainImageViews,
    VkRenderPass renderPass);

  ~SwapChainResources();

  std::vector<VkFramebuffer> getSwapChainFramebuffers() const {
    return swapChainFramebuffers;
  }

  void createFramebuffers();
  void recreateSwapChain();

  // Delete copy semantics
  SwapChainResources(const SwapChainResources&) = delete;
  SwapChainResources& operator=(const SwapChainResources&) = delete;
	// Custom move constructor
  SwapChainResources(SwapChainResources&& other) noexcept;
	// Custom move assignment
  SwapChainResources& operator=(SwapChainResources&& other) noexcept;
};

