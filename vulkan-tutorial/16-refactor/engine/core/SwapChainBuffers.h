#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../memory/Image.h"
#include "../memory/ImageView.h"

class SwapChainBuffers {
public:
  SwapChainBuffers(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkExtent2D swapChainExtent,
    VkSampleCountFlagBits msaaSamples,
    VkFormat colorFormat,
    VkFormat depthFormat,
    const std::vector<VkImageView>& swapChainImageViews,
    VkRenderPass renderPass);

  ~SwapChainBuffers();

  void recreateSwapChain();

  std::vector<VkFramebuffer> getSwapChainFramebuffers() const {
    return swapChainFramebuffers;
  }

  SwapChainBuffers(const SwapChainBuffers&) = delete;
  SwapChainBuffers& operator=(const SwapChainBuffers&) = delete;
  SwapChainBuffers(SwapChainBuffers&& other) noexcept;
  SwapChainBuffers& operator=(SwapChainBuffers&& other) noexcept;

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

