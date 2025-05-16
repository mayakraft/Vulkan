#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Buffers.h"

class Material {
public:
  // this is duplicated from Renderer, this needs to go away
	const int MAX_FRAMES_IN_FLIGHT = 2;

  Material(
    std::string texturePath,
    Device& device,
    Buffers& buffers,
    SwapChain& swapChain);

  ~Material();

  std::vector<VkDescriptorSet> getDescriptorSets() const { return descriptorSets; }

  void createDescriptorSets(
    VkDescriptorPool descriptorPool,
    std::vector<VkDescriptorSetLayout> layouts);

  void updateUniformBuffer(uint32_t currentImage);

private:
  Device& device;
  Buffers& buffers;
  SwapChain& swapChain;

  std::string texturePath;

  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void createUniformBuffers();

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  // descriptor sets are automatically freed when the descriptor pool is destroyed
  std::vector<VkDescriptorSet> descriptorSets;

  // texture
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
};

