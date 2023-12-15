#pragma once

// this one line is required to include the key:
// VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, or "VK_KHR_portability_subset"
// so if we don't include it, or if any problems arise, simply replace the
// macro with the string literal
#define VK_ENABLE_BETA_EXTENSIONS

// tell glfw that we want the additional Vulkan functionality
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Engine {

public:
	Engine();
	~Engine();
	void update();

private:
	const char *appName = "Triangle";
	const char *vertPath = "./simple.vert.spv";
	const char *fragPath = "./simple.frag.spv";

	// The cross-platform window visibile to the user
	GLFWwindow *window;

	// we only need one instance. clean up upon application exit.
	VkInstance instance;

	// physical device will be implicitly cleaned up on its own.
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	// logical devices will need to be cleaned up, before the instance itself
	VkDevice logicalDevice;

	// device queues are implicitly cleaned up on their own.
	VkQueue graphicsQueue;

	// file system. used for reading shader binary files.
	std::vector<char> readFile(const std::string &filepath);

	// heuristic method for choosing one of the possible many Vulkan devices
	VkPhysicalDevice chooseBestDevice(std::vector<VkPhysicalDevice> devices);

	// debug
	void printInfo(std::vector<const char *> extensions, int deviceCount);
};
