#include "SwapChainResources.h"

SwapChainResources::SwapChainResources(Device& device, SwapChain& swapChain, Pipeline& pipeline, Buffers& buffers)
  : device(device),
    swapChain(swapChain),
    pipeline(pipeline),
    buffers(buffers),
    colorImageNew(
      device.getDevice(),
      device.getPhysicalDevice(),
      swapChain.getSwapChainExtent().width,
      swapChain.getSwapChainExtent().height,
      1,
      device.getMsaaSamples(),
      swapChain.getSwapChainImageFormat(), // colorFormat,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    colorImageViewNew(
      device.getDevice(),
      colorImageNew.getImage(),
      swapChain.getSwapChainImageFormat(), // colorFormat,
      VK_IMAGE_ASPECT_COLOR_BIT,
      1),
    depthImageNew(
      device.getDevice(),
      device.getPhysicalDevice(),
      swapChain.getSwapChainExtent().width,
      swapChain.getSwapChainExtent().height,
      1,
      device.getMsaaSamples(),
      buffers.findDepthFormat(), // depthFormat
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    depthImageViewNew(
      device.getDevice(),
      depthImageNew.getImage(),
      buffers.findDepthFormat(), // depthFormat
      VK_IMAGE_ASPECT_DEPTH_BIT,
      1) {

  createFramebuffers();
}

SwapChainResources::~SwapChainResources() {
  for (auto framebuffer : swapChainFramebuffers) {
    vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
  }
}

void SwapChainResources::createFramebuffers() {
  swapChainFramebuffers.resize(swapChain.getSwapChainImageViews().size());

  for (size_t i = 0; i < swapChain.getSwapChainImageViews().size(); i++) {
    std::array<VkImageView, 3> attachments = {
      colorImageViewNew.getImageView(),
      depthImageViewNew.getImageView(),
      swapChain.getSwapChainImageViews()[i],
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = pipeline.getRenderPass();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChain.getSwapChainExtent().width;
    framebufferInfo.height = swapChain.getSwapChainExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer");
    }
  }
}

void SwapChainResources::recreateSwapChain() {
	// swapChain.recreateSwapChain(*this);

	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device.getDevice(), framebuffer, nullptr);
	}

  colorImageNew = Image(
    device.getDevice(),
    device.getPhysicalDevice(),
    swapChain.getSwapChainExtent().width,
    swapChain.getSwapChainExtent().height,
    1,
    device.getMsaaSamples(),
    swapChain.getSwapChainImageFormat(),
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  colorImageViewNew = ImageView(
    device.getDevice(),
    colorImageNew.getImage(),
    swapChain.getSwapChainImageFormat(), // colorFormat,
    VK_IMAGE_ASPECT_COLOR_BIT,
    1);

  VkFormat depthFormat = buffers.findDepthFormat();

  depthImageNew = Image(
    device.getDevice(),
    device.getPhysicalDevice(),
    swapChain.getSwapChainExtent().width,
    swapChain.getSwapChainExtent().height,
    1,
    device.getMsaaSamples(),
    depthFormat,
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  depthImageViewNew = ImageView(
    device.getDevice(),
    depthImageNew.getImage(),
    depthFormat,
    VK_IMAGE_ASPECT_DEPTH_BIT,
    1);

	createFramebuffers();
}

