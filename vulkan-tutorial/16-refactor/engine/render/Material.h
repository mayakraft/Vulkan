#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../core/Device.h"
#include "../core/SwapChain.h"
#include "../memory/Buffers.h"
#include "GraphicsPipeline.h"
#include "PipelineConfig.h"

class Renderer;

class Material {
public:
  Material(
    Device& device,
    Buffers& buffers,
    SwapChain& swapChain,
    Renderer& renderer,
    std::string texturePath);

  ~Material();

  VkPipeline getPipeline() const { return graphicsPipeline.get()->get(); }
  VkPipelineLayout getPipelineLayout() const { return graphicsPipeline.get()->getLayout(); }

  std::vector<VkDescriptorSet> getDescriptorSets() const { return descriptorSets; }
  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

  void createDescriptorSets();
  void createDescriptorSetLayout();
  void updateUniformBuffer(uint32_t currentImage);
  // essentially "recreateSwapChain"
  void updateExtent(VkExtent2D newExtent);

  PipelineConfig config;

  Material(const Material&) = delete;
  Material& operator=(const Material&) = delete;
  Material(Material&&) noexcept = default;
  Material& operator=(Material&&) noexcept;

private:
  // this is duplicated from Renderer, this needs to go away
	const int MAX_FRAMES_IN_FLIGHT = 2;

  Device& device;
  Buffers& buffers;
  SwapChain& swapChain;
  Renderer& renderer;

  std::unique_ptr<GraphicsPipeline> graphicsPipeline;

  // descriptor sets are used for shader uniforms
  // this is used to create pipelineLayout,
  // which in turn is used to create graphicsPipeline
  VkDescriptorSetLayout descriptorSetLayout;

  // descriptor sets are automatically freed when the descriptor pool is destroyed
  std::vector<VkDescriptorSet> descriptorSets;

  // uniforms
  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  // texture
  std::string texturePath;
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void createUniformBuffers();
};

