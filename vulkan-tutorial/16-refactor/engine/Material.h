#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Buffers.h"

class Material {
public:
  // this needs to go away
	const int MAX_FRAMES_IN_FLIGHT = 2;

  Material(Device& device, Buffers& buffers, SwapChain& swapChain); // , Pipeline& pipeline, VkDescriptorSetLayout layout);
  ~Material();

  /*void createTexture(const std::string& texturePath);*/
  void createTexture();
  void createUniformBuffers();

  VkDescriptorSet getDescriptorSet();
  VkImageView getTextureImageView();
  VkSampler getTextureSampler();

  // descriptor sets are automatically freed when the descriptor pool is destroyed
  std::vector<VkDescriptorSet> descriptorSets;

  void createDescriptorSets(
    VkDescriptorPool descriptorPool,
    std::vector<VkDescriptorSetLayout> layouts);

  void updateUniformBuffer(uint32_t currentImage);

private:
  Device& device;
  Buffers& buffers;
  SwapChain& swapChain;

  const std::string TEXTURE_PATH = "./assets/viking_room.png";

  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void createUniformBuffer();

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  // texture
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
};
