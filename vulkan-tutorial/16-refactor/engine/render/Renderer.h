#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../core/Device.h"
#include "../core/SwapChain.h"
#include "../core/SwapChainBuffers.h"
#include "../memory/Buffers.h"
#include "Model.h"
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

  Device& device;
  Buffers& buffers;
  SwapChain& swapChain;

  VkRenderPass renderPass;

  std::unique_ptr<SwapChainBuffers> swapChainBuffers;

  // command buffers are automatically freed when their command pool is destroyed
  std::vector<VkCommandBuffer> commandBuffers;

  // render objects, and their models and materials
  std::vector<RenderObject> renderObjects;
  std::vector<Model> models;
  std::vector<Material> materials;

  // Uniforms
  // descriptor pool is used to allocate descriptor sets
  // descriptor sets are currently owned by each Material
  VkDescriptorPool descriptorPool;

  // synchronization objects
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  // coordinate timing between the CPU and GPU
  // notably used here to reduce input latency
  std::vector<VkFence> inFlightFences;

  void createRenderPass();
  void createDescriptorPool();
  void createCommandBuffers();
  void createSyncObjects();

  // the other half of "drawFrame"
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  // if window attributes change this will be called
  // via. the public boolean frameBufferResized
  void recreateSwapChain();
};

