#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Vertex.h"

class Renderer {
public:
  Renderer(Device& device, SwapChain& swapChain, Pipeline& pipeline);
  ~Renderer();

  void drawFrame();

  std::vector<VkFramebuffer> getSwapChainFramebuffers() const { return swapChainFramebuffers;}

  bool framebufferResized = false;

private:
	const int MAX_FRAMES_IN_FLIGHT = 2;

  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void createVertexBuffer();
  void createIndexBuffer();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  void recreateSwapChain();

  Device& device;
  SwapChain& swapChain;
  Pipeline& pipeline;

  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  size_t currentFrame = 0;

  // this should become abstracted eventually
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{+0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{+0.5f, +0.5f}, {1.0f, 1.0f, 1.0f}},
    {{-0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}
  };

  // use uint32_t if you have more than 65535 vertices
  const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
  };

};
