#pragma once

#include "vkwindow.hpp"

namespace VulkanEngine {
	class App {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VKWindow window{WIDTH, HEIGHT, "Hello Vulkan"};
	};
}