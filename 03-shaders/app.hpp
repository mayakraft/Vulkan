#pragma once

#include "window.hpp"
#include "pipeline.hpp"

namespace VulkanEngine {

	class App {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		Window window{WIDTH, HEIGHT, "Hello Vulkan"};
		Pipeline pipeline{"simple.vert.spv", "simple.frag.spv"};
	};

}
