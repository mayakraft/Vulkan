#pragma once

#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vector>

class Device;

class Buffers {
private:
	Device& device;
	VkCommandPool commandPool;

  void createCommandPool();

public:
	Buffers(Device& device);
  ~Buffers();

  VkCommandPool getCommandPool() const { return commandPool; }

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// for the depth buffer
	VkFormat findSupportedFormat(
		Device& device,
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkFormat findDepthFormat(Device& device);

	void createImage(
		Device& device,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels, // mipmaps
		VkSampleCountFlagBits numSamples, // multisampling
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory);

	VkImageView createImageView(
		Device& device,
		VkImage image,
		VkFormat format,
		VkImageAspectFlags aspectFlags,
		uint32_t mipLevels);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void transitionImageLayout(
		Device& device,
		VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		uint32_t mipLevels);

	void generateMipmaps(
		Device& device,
		VkImage image,
		VkFormat imageFormat,
		uint32_t texWidth,
		uint32_t texHeight,
		uint32_t mipLevels);

	void copyBufferToImage(
		Device& device,
		VkBuffer buffer,
		VkImage image,
		uint32_t width,
		uint32_t height);

	void createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);
};
