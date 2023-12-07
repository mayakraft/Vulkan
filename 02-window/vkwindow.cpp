#include "vkwindow.hpp"

namespace VulkanEngine {

	VKWindow::VKWindow(
		int w,
		int h,
		std::string name) : width{w}, height{h}, windowName{name} {
		initWindow();
	}

	VKWindow::~VKWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VKWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

}
