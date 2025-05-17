#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Device.h"
#include "memory/Buffers.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "PipelineConfig.h"

class Renderer;

class Material {
public:
  // this is duplicated from Renderer, this needs to go away
	const int MAX_FRAMES_IN_FLIGHT = 2;

  Material(
    Device& device,
    Buffers& buffers,
    SwapChain& swapChain,
    Renderer& renderer,
    std::string texturePath);

  ~Material();

  /*std::vector<VkDescriptorSet> getDescriptorSets() const { return descriptorSets; }*/
  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
  /*std::unique_ptr<GraphicsPipeline> getGraphicsPipeline() const { return graphicsPipeline; }*/

  VkPipeline getPipeline() const { return graphicsPipeline.get()->get(); }
  VkPipelineLayout getPipelineLayout() const { return graphicsPipeline.get()->getLayout(); }

  /*void createDescriptorSets(*/
  /*  VkDescriptorPool descriptorPool,*/
  /*  std::vector<VkDescriptorSetLayout> layouts);*/
  void createDescriptorSets();
  void createDescriptorSetLayout();

  void updateUniformBuffer(uint32_t currentImage);

  // essentially "recreateSwapChain"
  void updateExtent(VkExtent2D newExtent);

  PipelineConfig config;

  // descriptor sets are automatically freed when the descriptor pool is destroyed
  std::vector<VkDescriptorSet> descriptorSets;

  Material(const Material&) = delete;
  Material& operator=(const Material&) = delete;

  Material(Material&&) noexcept = default;
  /*Material& operator=(Material&&) noexcept = default;*/

private:
  std::string texturePath;

  Device& device;
  Buffers& buffers;
  SwapChain& swapChain;
  Renderer& renderer;

  // GraphicsPipeline& graphicsPipeline;
  std::unique_ptr<GraphicsPipeline> graphicsPipeline;

  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void createUniformBuffers();

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  // descriptor sets are used for shader uniforms
  // this is used to create pipelineLayout,
  // which in turn is used to create graphicsPipeline
  VkDescriptorSetLayout descriptorSetLayout;

  // texture
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
};

