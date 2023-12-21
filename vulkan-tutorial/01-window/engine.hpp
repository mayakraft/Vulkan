#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Engine {

public:
	Engine();
	~Engine();
	void startLoop();

private:
	GLFWwindow *window;
};
