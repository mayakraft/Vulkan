#include "app.hpp"

namespace VulkanEngine {

	void App::run() {
		while (!window.shouldClose()) {
			glfwPollEvents();
		}
	}

}
