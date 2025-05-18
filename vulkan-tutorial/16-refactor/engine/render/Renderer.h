#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../core/Device.h"
#include "../core/SwapChain.h"
#include "../core/SwapChainResources.h"
#include "../memory/Buffers.h"
#include "../memory/Image.h"
#include "../memory/ImageView.h"
#include "Material.h"
#include "RenderObject.h"

class Renderer {
public:
  Renderer(Device& device, SwapChain& swapChain, Buffers& buffers);
  ~Renderer();

  void drawFrame();

  VkRenderPass getRenderPass() const { return renderPass; }
  VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

  bool framebufferResized = false;

private:
	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;

  void createFramebuffers();
  void createCommandBuffers();
  void createSyncObjects();
  void createColorResources();
  void createDepthResources();
  void createDescriptorPool();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  // if window attributes change this will be called
  void recreateSwapChain();

  Device& device;
  SwapChain& swapChain;
  Buffers& buffers;
  std::unique_ptr<SwapChainResources> swapChainResources;

  VkRenderPass renderPass;
  void createRenderPass();

  // command buffers are automatically freed when their command pool is destroyed
  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  // mesh objects
  std::vector<RenderObject> renderObjects;

  // new materials
  std::vector<Material> materials;

  // Uniforms
  // descriptor pool is used to allocate descriptor sets
  // descriptor sets are currently owned by each Material
  VkDescriptorPool descriptorPool;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;

  // coordinate timing between the CPU and GPU
  // notably used here to reduce input latency
  std::vector<VkFence> inFlightFences;
};

