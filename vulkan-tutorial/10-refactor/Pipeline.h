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

  const char *vertPath = "./simple.vert.spv";
	const char *fragPath = "./simple.frag.spv";

private:
  void createRenderPass();
  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char>& code);
  std::vector<char> readFile(const std::string& filename);

  Device& device;
  SwapChain& swapChain;

  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
};
