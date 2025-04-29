#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "Device.h"
#include "SwapChain.h"
#include "Vertex.h"

class Pipeline {
public:
  Pipeline(Device& device, SwapChain& swapChain);
  ~Pipeline();

  VkPipeline getGraphicsPipeline() const { return graphicsPipeline; }
  VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
  VkRenderPass getRenderPass() const { return renderPass; }
  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

  const char *vertPath = "./shaders/simple.vert.spv";
	const char *fragPath = "./shaders/simple.frag.spv";

private:
  void createRenderPass();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char>& code);
  std::vector<char> readFile(const std::string& filename);

  Device& device;
  SwapChain& swapChain;

  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  // both of these are duplicated from Renderer class
  VkFormat findDepthFormat();
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};
