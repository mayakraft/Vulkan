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

  // command buffers are automatically freed when their command pool is destroyed
  std::vector<VkCommandBuffer> commandBuffers;

  // the mesh geometry to be rendered
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

  // Uniforms
  // descriptor pool is used to allocate descriptor sets
  VkDescriptorPool descriptorPool;
  // descriptor sets are automatically freed when the descriptor pool is destroyed
  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
};
