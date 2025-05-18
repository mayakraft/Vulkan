#include <stdexcept>
#include "ImageView.h"

VkImageView ImageView::CreateImageView(
  VkDevice device,
  VkImage image,
  VkFormat format,
  VkImageAspectFlags aspectFlags,
  uint32_t mipLevels) {

  VkImageView imageView;
	VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view");
  }
  return imageView;
}

ImageView::ImageView(
  VkDevice device,
  VkImage image,
  VkFormat format,
  VkImageAspectFlags aspectFlags,
  uint32_t mipLevels) : device(device) {
  imageView = ImageView::CreateImageView(device, image, format, aspectFlags, mipLevels);
}

ImageView::~ImageView() {
  if (imageView != VK_NULL_HANDLE) {
    vkDestroyImageView(device, imageView, nullptr);
  }
}

// Custom move constructor
ImageView::ImageView(ImageView&& other) noexcept
	: device(other.device),
	  imageView(other.imageView) {
  other.device = VK_NULL_HANDLE;
	other.imageView = VK_NULL_HANDLE;
}

// Move assignment
ImageView& ImageView::operator=(ImageView&& other) noexcept {
  if (this != &other) {
    if (imageView != VK_NULL_HANDLE) {
      vkDestroyImageView(device, imageView, nullptr);
    }

    device = other.device;
    imageView = other.imageView;

    other.device = VK_NULL_HANDLE;
    other.imageView = VK_NULL_HANDLE;
  }
  return *this;
}

