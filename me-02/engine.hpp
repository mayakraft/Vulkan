#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
};

class Engine {

public:
	Engine();
	~Engine();
	void update();

private:
	const char *appName = "Triangle";
	const char *vertPath = "./simple.vert.spv";
	const char *fragPath = "./simple.frag.spv";
	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char *> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		"VK_KHR_portability_subset", // VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
	};

	VkInstance instance;
	GLFWwindow *window;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;

	std::vector<char> readFile(const std::string &filepath);
	VkPhysicalDevice chooseBestDevice(std::vector<VkPhysicalDevice> devices);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
