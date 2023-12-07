#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace VulkanEngine {

	class VKWindow {

	public:
		VKWindow(int w, int h, std::string name);
		~VKWindow();

		VKWindow(const VKWindow &) = delete;
		VKWindow &operator=(const VKWindow &) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

	private:
		void initWindow();

		const int width;
		const int height;

		std::string windowName;
		GLFWwindow *window;
	};

}
