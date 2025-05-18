#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

class ImageView {
public:
  static VkImageView CreateImageView(
		VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    uint32_t mipLevels);

  ImageView(
		VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    uint32_t mipLevels);

  ~ImageView();

  VkImageView getImageView() const { return imageView; }

	ImageView(const ImageView&) = delete;
	ImageView& operator=(const ImageView&) = delete;
	ImageView(ImageView&& other) noexcept;
	ImageView& operator=(ImageView&& other) noexcept;

private:
	VkDevice device;
	VkPhysicalDevice physicalDevice;
  VkImageView imageView = VK_NULL_HANDLE;
};

