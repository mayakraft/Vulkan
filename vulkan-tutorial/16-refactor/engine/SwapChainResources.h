#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Image.h"
#include "ImageView.h"

class SwapChainResources {
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

  void recreateSwapChain();

  std::vector<VkFramebuffer> getSwapChainFramebuffers() const {
    return swapChainFramebuffers;
  }

  // Delete copy semantics
  SwapChainResources(const SwapChainResources&) = delete;
  SwapChainResources& operator=(const SwapChainResources&) = delete;
	// Custom move constructor
  SwapChainResources(SwapChainResources&& other) noexcept;
	// Custom move assignment
  SwapChainResources& operator=(SwapChainResources&& other) noexcept;

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

  void deallocAll();
  void createFramebuffers();
};

