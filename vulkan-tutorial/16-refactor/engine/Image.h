#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

class Image {
public:
  Image(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels, // mipmaps
		VkSampleCountFlagBits numSamples, // multisampling
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties);

  ~Image();

  VkImage getImage() const { return image; }

  // Delete copy semantics
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	// Custom move constructor
	Image(Image&& other) noexcept;
	// Custom move assignment
	Image& operator=(Image&& other) noexcept;

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
  VkImage image = VK_NULL_HANDLE;
  VkDeviceMemory memory = VK_NULL_HANDLE;

  // this is duplicated in Buffers.h
  uint32_t findMemoryType(
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties);
};

