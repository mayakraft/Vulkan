#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "game_object.hpp"

#include <memory>
#include <vector>

namespace VulkanEngine {

	class SimpleRenderSystem {

	public:
		SimpleRenderSystem(Device &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem &) = delete;
		SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
		void renderGameObjects(
			VkCommandBuffer commandBuffer,
			std::vector<GameObject> &gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		Device &device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};

}
