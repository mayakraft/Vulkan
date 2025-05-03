#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

class ImageView {
private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
  VkImageView imageView = VK_NULL_HANDLE;

public:
  ImageView() = default;

  ImageView(
		VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    uint32_t mipLevels);

  ~ImageView();

  // Delete copy semantics
	ImageView(const ImageView&) = delete;
	ImageView& operator=(const ImageView&) = delete;

	// Custom move constructor
	ImageView(ImageView&& other) noexcept;

	// Custom move assignment
	ImageView& operator=(ImageView&& other) noexcept;

  VkImageView getImageView() const { return imageView; }
};

