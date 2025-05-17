#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <cstdint>

typedef struct PipelineConfig {
  std::string vertPath;
  std::string fragPath;
  VkRenderPass renderPass = VK_NULL_HANDLE;
  VkExtent2D extent;
  VkSampleCountFlagBits msaaSamples;
  VkDescriptorSetLayout descriptorSetLayout;
} PipelineConfig;

