#include "engine.hpp"

Engine::Engine() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(512, 512, "Vulkan", nullptr, nullptr);
}

Engine::~Engine() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Engine::update() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}
