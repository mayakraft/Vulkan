#include "Renderer.h"
#include "Buffers.h"
#include <stdexcept>
#include "Uniforms.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <array>

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->framebufferResized = true;
}

bool hasStencilComponent(VkFormat format) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

Renderer::Renderer(Device& device, SwapChain& swapChain, Buffers& buffers, Pipeline& pipeline)
  : device(device),
    swapChain(swapChain),
    buffers(buffers),
    pipeline(pipeline),
    swapChainResources(
      device.getDevice(),
      device.getPhysicalDevice(),
      swapChain.getSwapChainExtent(),
      device.getMsaaSamples(),
      swapChain.getSwapChainImageFormat(),
      buffers.findDepthFormat(),
      swapChain.getSwapChainImageViews(),
      pipeline.getRenderPass()) {

  // create the viking room example
  materials.emplace_back("./assets/viking_room.png", device, buffers, swapChain);
  renderObjects.emplace_back("./assets/viking_room.obj", device, buffers, materials[0]);

  createDescriptorPool();

  // this got moved out of the createDescriptorSets
  std::vector<VkDescriptorSetLayout> layouts(
    MAX_FRAMES_IN_FLIGHT,
    pipeline.getDescriptorSetLayout());
  for (auto& material : materials) {
    material.createDescriptorSets(descriptorPool, layouts);
  }
  createCommandBuffers();
  createSyncObjects();

  glfwSetWindowUserPointer(device.getWindow(), this);
	glfwSetFramebufferSizeCallback(device.getWindow(), framebufferResizeCallback);
}

Renderer::~Renderer() {
  vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device.getDevice(), renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(device.getDevice(), imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(device.getDevice(), inFlightFences[i], nullptr);
  }
}

void Renderer::recreateSwapChain() {
	swapChain.recreateSwapChain();
  // note: we are not recreating the render pass.
  // if the app was moved between two monitors with different
  // dynamic ranges, it would be better to recreate the render pass.
  // the render pass is owned by Pipeline()
  swapChainResources = SwapChainResources(
    device.getDevice(),
    device.getPhysicalDevice(),
    swapChain.getSwapChainExtent(),
    device.getMsaaSamples(),
    swapChain.getSwapChainImageFormat(),
    buffers.findDepthFormat(),
    swapChain.getSwapChainImageViews(),
    pipeline.getRenderPass());
}

// descriptor sets cannot be allocated directly they must be allocated from a pool,
// just like how command buffers are allocated.
// this is called in preparation to calling the create descriptor sets function.
void Renderer::createDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool");
  }
}

void Renderer::createCommandBuffers() {
  // commandBuffers.resize(swapChainFramebuffers.size());
  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = device.getCommandPool();
  // there are two possible levels:
  // - VK_COMMAND_BUFFER_LEVEL_PRIMARY
  // - VK_COMMAND_BUFFER_LEVEL_SECONDARY
  // the secondary ones cannot be submitted directly but are instead
  // called from primary command buffers
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

  if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers");
  }
}

void Renderer::createSyncObjects() {
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
      vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
      vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame");
    }
  }
}

void Renderer::drawFrame() {
  // using a fence to reduce the latency between the CPU and GPU,
  // for example, user input via keyboard or mouse comes in as frames
  // are being rendered in the background before being presented to the screen,
  // therefore the user input and screen become out of sync.
  // this fence tells the GPU to wait a bit longer if needed so as to
  // create more of a linear sequence of drawing and presenting.
  vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

  // ask the swap chain for the next available image that we can write into
  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
    device.getDevice(),
    swapChain.getSwapChain(),
    UINT64_MAX, // timeout (max means potentially wait forever)
    imageAvailableSemaphores[currentFrame],
    VK_NULL_HANDLE,
    &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
  	recreateSwapChain();
  	return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }

  // only reset the fence if we are submitting work
  vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

  for (auto& material : materials) material.updateUniformBuffer(currentFrame);

  vkResetCommandBuffer(commandBuffers[currentFrame], 0);
  recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

  // to draw, the rendering workload is submitted to the queue
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

  // this semaphor indicates that the rendering has completed
  // and the image can now be handed off to the swap chain
  VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  // submit the rendering workflow to the queue
  // the final parameter (fence) is used to sync the CPU with the GPU
  if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer");
  }

  // now onto presenting the rendering to the screen
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  // the aformentioned semaphor, indicating that the rendering has finished
  // and the image is ready to be handed off to the swap chain
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = { swapChain.getSwapChain() };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  // submit the presentation instruction
  vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image");
	}

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer");
  }

  VkExtent2D swapChainExtent = swapChain.getSwapChainExtent();

  // before adding depth buffer, we only needed the one clear value
  // VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};

  // drawing starts by configuring the render pass.
  // attach the frame buffer for the correct swap chain image,
  // and some values which define the size of the render area / clear color values.
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline.getRenderPass();
  renderPassInfo.framebuffer = swapChainResources.getSwapChainFramebuffers()[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChainExtent;
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  // we could be executing this beginning to the render pass with one of two flags:
  // - VK_SUBPASS_CONTENTS_INLINE
  // - VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  // bind the graphics pipeline
  // the second parameter specifies if the pipeline is graphics or compute
  vkCmdBindPipeline(
    commandBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    pipeline.getGraphicsPipeline());

  VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // per-object draw call moved in here
  for (auto& object : renderObjects) {
    object.recordCommandBuffer(
      commandBuffer,
      pipeline.getPipelineLayout(),
      currentFrame
    );
  }

	vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}

