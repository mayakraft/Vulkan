#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "../core/SwapChain.h"
#include "../memory/Buffers.h"
#include "../geometry/Vertex.h"
#include "PipelineConfig.h"

class GraphicsPipeline {
public:
  GraphicsPipeline(VkDevice device, const PipelineConfig& config);
  ~GraphicsPipeline();

  VkPipeline get() const { return graphicsPipeline; }
  VkPipelineLayout getLayout() const { return pipelineLayout; }

private:
  VkDevice device;
  VkPipeline graphicsPipeline;
  VkPipelineLayout pipelineLayout;

  void createGraphicsPipeline(const PipelineConfig& config);
  VkShaderModule createShaderModule(const std::vector<char>& code);
  std::vector<char> readFile(const std::string& filename);
};

