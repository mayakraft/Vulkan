#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "SwapChain.h"
#include "Buffers.h"
#include "Image.h"
#include "ImageView.h"
#include "Pipeline.h"
#include "Vertex.h"
#include "SwapChainResources.h"

class Renderer {
public:
  Renderer(Device& device, SwapChain& swapChain, Buffers& buffers, Pipeline& pipeline);
  ~Renderer();

  void drawFrame();

  // std::vector<VkFramebuffer> getSwapChainFramebuffers() const { return swapChainFramebuffers; }

  bool framebufferResized = false;

private:
	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;

  const std::string MODEL_PATH = "./assets/viking_room.obj";
  const std::string TEXTURE_PATH = "./assets/viking_room.png";

  void loadModel();
  void createFramebuffers();
  void createCommandBuffers();
  void createSyncObjects();
  void createColorResources();
  void createDepthResources();
  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void updateUniformBuffer(uint32_t currentImage);

  void recreateSwapChain();

  Device& device;
  SwapChain& swapChain;
  Buffers& buffers;
  Pipeline& pipeline;

  SwapChainResources swapChainResources;

  std::vector<VkCommandBuffer> commandBuffers;
  // std::vector<VkFramebuffer> swapChainFramebuffers;

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  // texture
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
};
