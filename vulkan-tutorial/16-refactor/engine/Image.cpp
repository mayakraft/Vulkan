#include "Image.h"
#include <stdexcept>

Image::Image(
	VkDevice device,
	VkPhysicalDevice physicalDevice,
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	VkSampleCountFlagBits numSamples,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties) : device(device), physicalDevice(physicalDevice) {

	VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = numSamples;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.flags = 0; // Optional

  if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory");
  }

  vkBindImageMemory(device, image, memory, 0);
}

Image::~Image() {
  if (image != VK_NULL_HANDLE) {
    vkDestroyImage(device, image, nullptr);
  }
  if (memory != VK_NULL_HANDLE) {
    vkFreeMemory(device, memory, nullptr);
  }
}
// Custom move constructor
Image::Image(Image&& other) noexcept
	: device(other.device),
	  physicalDevice(other.physicalDevice),
	  image(other.image),
	  memory(other.memory) {
  other.device = VK_NULL_HANDLE;
  other.physicalDevice = VK_NULL_HANDLE;
	other.image = VK_NULL_HANDLE;
	other.memory = VK_NULL_HANDLE;
}

// Move assignment
Image& Image::operator=(Image&& other) noexcept {
  if (this != &other) {
    // Clean up existing resources
    if (image != VK_NULL_HANDLE) {
      vkDestroyImage(device, image, nullptr);
    }
    if (memory != VK_NULL_HANDLE) {
      vkFreeMemory(device, memory, nullptr);
    }

    device = other.device;
    physicalDevice = other.physicalDevice;
    image = other.image;
    memory = other.memory;

    other.device = VK_NULL_HANDLE;
    other.physicalDevice = VK_NULL_HANDLE;
    other.image = VK_NULL_HANDLE;
    other.memory = VK_NULL_HANDLE;
  }
  return *this;
}

uint32_t Image::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type");
}
