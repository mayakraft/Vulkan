#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "Device.h"
#include "SwapChain.h"
#include "Buffers.h"
#include "Vertex.h"

class Pipeline {
public:
  Pipeline(Device& device, SwapChain& swapChain, Buffers& buffers);
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
  Buffers& buffers;

  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  // descriptor sets are used for shader uniforms
  // this is used to create pipelineLayout,
  // which in turn is used to create graphicsPipeline
  VkDescriptorSetLayout descriptorSetLayout;
};

