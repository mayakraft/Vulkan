#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace VulkanEngine {

	class Renderer {

	public:
		Renderer(Window &window, Device &device);
		~Renderer();

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

		bool isFrameInProgress() const { return isFrameStarted; }

		VkRenderPass getSwapChainRenderPass() const {
			return swapChain->getRenderPass();
		}
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		Window& window;
		Device& device;
		std::unique_ptr<SwapChain> swapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};

}
