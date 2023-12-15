#include "engine.hpp"

/**
 * Initialize everything
 */
Engine::Engine() {
	//
	// GLFW
	//
	glfwInit();

	// by default, GLFW initializes with OpenGL. we won't be using OpenGL.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// resizing is an entire ordeal. turn it off for now.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// the fourth param is which monitor to open the window.
	// the last param is specific to OpenGL. ignore this.
	window = glfwCreateWindow(512, 512, "Vulkan", nullptr, nullptr);
}

/**
 * Destroy and cleanup memory
 */
Engine::~Engine() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

/**
 * Draw loop
 */
void Engine::update() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}
