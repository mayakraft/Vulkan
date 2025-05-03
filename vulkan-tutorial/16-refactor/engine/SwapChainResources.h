#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Buffers.h"
#include "Image.h"
#include "ImageView.h"

class SwapChainResources {
private:
  Device& device;
  SwapChain& swapChain;
  Pipeline& pipeline;
  Buffers& buffers;

  // color image
  Image colorImageNew;
  ImageView colorImageViewNew;

  // depth
  Image depthImageNew;
  ImageView depthImageViewNew;

  // framebuffers
  std::vector<VkFramebuffer> swapChainFramebuffers;

public:
  SwapChainResources(Device& device, SwapChain& swapChain, Pipeline& pipeline, Buffers& buffers);

  ~SwapChainResources();

  std::vector<VkFramebuffer> getSwapChainFramebuffers() const { return swapChainFramebuffers; }

  void createFramebuffers();
  void recreateSwapChain();
};

