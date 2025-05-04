#pragma once

#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vector>

class Device;

class Buffers {
private:
	Device& device;

public:
	Buffers(Device& device): device(device) {}
  ~Buffers() {}

	void createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory);

	void copyBuffer(
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size);

	void copyBufferToImage(
		VkBuffer buffer,
		VkImage image,
		uint32_t width,
		uint32_t height);

	void createImage(
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
		VkImage image,
		VkFormat format,
		VkImageAspectFlags aspectFlags,
		uint32_t mipLevels);

	void transitionImageLayout(
		VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		uint32_t mipLevels);

	void generateMipmaps(
		VkImage image,
		VkFormat imageFormat,
		uint32_t texWidth,
		uint32_t texHeight,
		uint32_t mipLevels);

	uint32_t findMemoryType(
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

	// for the depth buffer
	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkFormat findDepthFormat();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
};
