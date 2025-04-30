#pragma once

#include <vulkan/vulkan.h>
#include <string>

struct PipelineConfig {
  VkRenderPass renderPass;
  VkExtent2D extent;
  VkDescriptorSetLayout descriptorSetLayout;
  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
  VkFormat colorFormat;
  VkFormat depthFormat;
  std::string vertShaderPath = "./shaders/simple.vert.spv";
  std::string fragShaderPath = "./shaders/simple.frag.spv";
};
