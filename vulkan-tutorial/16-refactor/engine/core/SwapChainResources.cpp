#include "SwapChainResources.h"

SwapChainResources::SwapChainResources(
  VkDevice device,
  VkPhysicalDevice physicalDevice,
  VkExtent2D swapChainExtent,
  VkSampleCountFlagBits msaaSamples,
  VkFormat colorFormat,
  VkFormat depthFormat,
  const std::vector<VkImageView>& swapChainImageViews,
  VkRenderPass renderPass)
  : device(device),
    physicalDevice(physicalDevice),
    swapChainExtent(swapChainExtent),
    msaaSamples(msaaSamples),
    colorFormat(colorFormat),
    depthFormat(depthFormat),
    swapChainImageViews(swapChainImageViews),
    renderPass(renderPass),
    colorImage(
      device,
      physicalDevice,
      swapChainExtent.width,
      swapChainExtent.height,
      1,
      msaaSamples,
      colorFormat,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    colorImageView(
      device,
      colorImage.getImage(),
      colorFormat,
      VK_IMAGE_ASPECT_COLOR_BIT,
      1),
    depthImage(
      device,
      physicalDevice,
      swapChainExtent.width,
      swapChainExtent.height,
      1,
      msaaSamples,
      depthFormat,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    depthImageView(
      device,
      depthImage.getImage(),
      depthFormat,
      VK_IMAGE_ASPECT_DEPTH_BIT,
      1) {

  createFramebuffers();
}

SwapChainResources::~SwapChainResources() {
  deallocAll();
}

void SwapChainResources::deallocAll() {
  for (auto framebuffer : swapChainFramebuffers) {
    if (framebuffer != VK_NULL_HANDLE) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
  }
}

void SwapChainResources::createFramebuffers() {
  swapChainFramebuffers.resize(swapChainImageViews.size());

  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    std::array<VkImageView, 3> attachments = {
      colorImageView.getImageView(),
      depthImageView.getImageView(),
      swapChainImageViews[i],
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer");
    }
  }
}

void SwapChainResources::recreateSwapChain() {
  deallocAll();

  colorImage = Image(
    device,
    physicalDevice,
    swapChainExtent.width,
    swapChainExtent.height,
    1,
    msaaSamples,
    colorFormat,
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  colorImageView = ImageView(
    device,
    colorImage.getImage(),
    colorFormat,
    VK_IMAGE_ASPECT_COLOR_BIT,
    1);

  depthImage = Image(
    device,
    physicalDevice,
    swapChainExtent.width,
    swapChainExtent.height,
    1,
    msaaSamples,
    depthFormat,
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  depthImageView = ImageView(
    device,
    depthImage.getImage(),
    depthFormat,
    VK_IMAGE_ASPECT_DEPTH_BIT,
    1);

	createFramebuffers();
}

SwapChainResources::SwapChainResources(SwapChainResources&& other) noexcept
  : device(other.device),
    physicalDevice(other.physicalDevice),
    swapChainExtent(other.swapChainExtent),
    msaaSamples(other.msaaSamples),
    colorFormat(other.colorFormat),
    depthFormat(other.depthFormat),
    swapChainImageViews(other.swapChainImageViews),
    renderPass(other.renderPass),
    colorImage(std::move(other.colorImage)),
    colorImageView(std::move(other.colorImageView)),
    depthImage(std::move(other.depthImage)),
    depthImageView(std::move(other.depthImageView)),
    swapChainFramebuffers(std::move(other.swapChainFramebuffers)) {

  // Clear other's framebuffers so it doesn't try to destroy them
  other.swapChainFramebuffers.clear();
}

SwapChainResources& SwapChainResources::operator=(SwapChainResources&& other) noexcept {
  if (this != &other) {
    // Clean up existing framebuffers
    for (auto framebuffer : swapChainFramebuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    device = other.device;
    physicalDevice = other.physicalDevice;
    swapChainExtent = other.swapChainExtent;
    msaaSamples = other.msaaSamples;
    colorFormat = other.colorFormat;
    depthFormat = other.depthFormat;
    renderPass = other.renderPass;

    // it's safe to assign a reference to a const vector since we're not owning it
    const_cast<std::vector<VkImageView>&>(swapChainImageViews) = other.swapChainImageViews;

    colorImage = std::move(other.colorImage);
    colorImageView = std::move(other.colorImageView);
    depthImage = std::move(other.depthImage);
    depthImageView = std::move(other.depthImageView);
    swapChainFramebuffers = std::move(other.swapChainFramebuffers);

    // Clear other's framebuffers so it doesn't try to destroy them
    other.swapChainFramebuffers.clear();
  }
  return *this;
}

