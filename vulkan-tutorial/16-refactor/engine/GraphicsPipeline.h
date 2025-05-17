#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "Device.h"
#include "SwapChain.h"
#include "memory/Buffers.h"
#include "Vertex.h"
#include "PipelineConfig.h"

class GraphicsPipeline {
public:
  // PipelineBuilder(Device& device, SwapChain& swapChain, Buffers& buffers);
  GraphicsPipeline(Device& device, const PipelineConfig& config);
  ~GraphicsPipeline();

  VkPipeline get() const { return graphicsPipeline; }
  VkPipelineLayout getLayout() const { return pipelineLayout; }

  /*VkPipeline getGraphicsPipeline() const { return graphicsPipeline; }*/
  /*VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }*/
  /*VkRenderPass getRenderPass() const { return renderPass; }*/
  /*VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }*/

private:
  Device& device;
  /*SwapChain& swapChain;*/
  /*Buffers& buffers;*/
  VkPipeline graphicsPipeline;
  VkPipelineLayout pipelineLayout;
  /*VkRenderPass renderPass;*/

  void createGraphicsPipeline(const PipelineConfig& config);
  VkShaderModule createShaderModule(const std::vector<char>& code);
  std::vector<char> readFile(const std::string& filename);

  /*void createRenderPass();*/
  /*void createDescriptorSetLayout();*/
  /*void createGraphicsPipeline();*/
  /*VkShaderModule createShaderModule(const std::vector<char>& code);*/
  /*std::vector<char> readFile(const std::string& filename);*/
};

