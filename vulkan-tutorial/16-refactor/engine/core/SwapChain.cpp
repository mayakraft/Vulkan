#include <stdexcept>
#include <algorithm>
#include "SwapChain.h"
#include "../memory/ImageView.h"
#include "Debug.h"

SwapChain::SwapChain(Device& device) : device(device) {
  createSwapChain();
  createImageViews();
}

SwapChain::~SwapChain() {
  deallocAll();
}

// make sure not to call this until we know these resources
// are no longer in use (after vkDeviceWaitIdle)
void SwapChain::deallocAll() {
  for (auto imageView : swapChainImageViews) {
    vkDestroyImageView(device.getDevice(), imageView, nullptr);
  }
  vkDestroySwapchainKHR(device.getDevice(), swapChain, nullptr);
}

// this is one half of the code necessary to recreate a swap chain
// the other half lives on the Renderer class.
void SwapChain::recreateSwapChain() {
  DEBUG_LOG("recreate swap chain");
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(device.getWindow(), &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(device.getWindow(), &width, &height);
		glfwWaitEvents();
	}

  // we are about to free a bunch of resources, but if they are
  // still in use the program will crash. wait until the device is idle
	vkDeviceWaitIdle(device.getDevice());

  deallocAll();

	// recreate swap chain
	createSwapChain();
	createImageViews();
}

void SwapChain::createSwapChain() {
  // Query swap chain support details
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    device.getPhysicalDevice(),
    device.getSurface(),
    &capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(
    device.getPhysicalDevice(),
    device.getSurface(),
    &formatCount,
    nullptr);

  if (formatCount == 0) {
    throw std::runtime_error("failed to find supported surface formats");
  }

  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(
    device.getPhysicalDevice(),
    device.getSurface(),
    &formatCount,
    formats.data());

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(
    device.getPhysicalDevice(),
    device.getSurface(),
    &presentModeCount,
    nullptr);

  if (presentModeCount == 0) {
    throw std::runtime_error("failed to find supported present modes");
  }

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
    device.getPhysicalDevice(),
    device.getSurface(),
    &presentModeCount,
    presentModes.data());

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes);
	VkExtent2D extent = chooseSwapExtent(capabilities);
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

  // this will set how many images the swap chain will have.
  // we can keep it minimal, however if we set it to the minimum there is potential
  // for unnecessary waiting before acquiring another image, so prefer minimum + 1,
  // unless minimum + 1 exceeds the maximum count, in which case, set it to the maximum.
	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0
		&& imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	uint32_t queueFamilyIndices[] = {
		device.getGraphicsQueueFamilyIndex(),
		device.getPresentQueueFamilyIndex()
	};
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = device.getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
  // tell the swap chain that these images will be rendered directly
  // into a graphics pipeline.
  // Alternatively, it could be
  // - TRANSFER_DESTINATION: images are pre-written elsewhere and
  //   then copied/blipped into this swap chain's set of images
  // - STORAGE: the image is written by a compute shader
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // on integrated GPUs (MacOS M-series for example), these two
  // are the same, in which case we need to be explicit and only
  // set sharing mode to exclusive (which is faster anyway).
  if (queueFamilyIndices[0] == queueFamilyIndices[1]) {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device.getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain");
  }

  // we only specified a minimum number of swap chain images,
  // not the absolute amount, it's possible it makes more.
  // we need to query how many images were created and resize the container.
  vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device.getDevice(), swapChain, &imageCount, swapChainImages.data());
}

// todo: this method could be simplified to call the class ImageView
// inside of a loop, but Renderer is not accessible from here.
// https://vulkan-tutorial.com/en/Texture_mapping/Image_view_and_sampler
void SwapChain::createImageViews() {
  swapChainImageViews.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    swapChainImageViews[i] = ImageView::CreateImageView(
      device.getDevice(),
      swapChainImages[i],
      swapChainImageFormat,
      VK_IMAGE_ASPECT_COLOR_BIT,
      1);
  }
}

// When we create the swap chain we need to give it a color space and format,
// we have a preferred pair if they exist, otherwise
// return the first available color space and format.
VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  // we could rank these by a best option, but in most cases simply returning
  // the first one which satisfies these two constraints is perfectly fine.
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

// Perhaps the most important setting for the swap chain, this sets the conditions
// for how the swap chain should show images to the screen. there are four options:
// - VK_PRESENT_MODE_IMMEDIATE_KHR transfer images to the screen ASAP even if
//   it results in screen tearing.
// - VK_PRESENT_MODE_FIFO_KHR images go into a FIFO queue, if the queue is full the program waits,
//   this is the most similar to the vertical sync as found in most modern video games.
//   Note: this is the only one which is guaranteed to be available.
// - VK_PRESENT_MODE_FIFO_RELAXED_KHR similar to FIFO but in the case where the app is late when
//   the queue is empty, it will show the image ASAP and may result in screen tearing.
// - VK_PRESENT_MODE_MAILBOX_KHR again similar to FIFO but if the queue is full, new images will
//   be added but will replace the last item on the queue, resulting in fewer latency issues.
//   Note: if exists, this is preferred.
VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

// This will get the actual resolution in pixels which will be drawn.
// First we consult the pre-calculated capabilities, if valid, return that.
// Sometimes this duty will fall on us, in which case capabilities will be set to the max uint32.
// If this happens we have to rely on glfw to get the width and the height because
// on some systems (MacOS retina displays) pixels and screen coordinates don't align,
// in this case we glfw will help us to get the actual pixels, which is what we need.
VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(device.getWindow(), &width, &height);

    VkExtent2D actualExtent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };
    // std::cout << "actualExtent " << width << " " << height << std::endl;

    actualExtent.width = std::clamp(
      actualExtent.width,
      capabilities.minImageExtent.width,
      capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
      actualExtent.height,
      capabilities.minImageExtent.height,
      capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

