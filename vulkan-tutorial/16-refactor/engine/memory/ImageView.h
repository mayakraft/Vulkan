#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

class ImageView {
public:
  ImageView(
		VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    uint32_t mipLevels);

  ~ImageView();

  VkImageView getImageView() const { return imageView; }

  // Delete copy semantics
	ImageView(const ImageView&) = delete;
	ImageView& operator=(const ImageView&) = delete;
	// Custom move constructor
	ImageView(ImageView&& other) noexcept;
	// Custom move assignment
	ImageView& operator=(ImageView&& other) noexcept;

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
  VkImageView imageView = VK_NULL_HANDLE;
};

