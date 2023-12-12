#include "engine.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>

Engine::Engine() {
	// glfw
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(512, 512, appName, nullptr, nullptr);

	// shaders
	auto vertCode = readFile(vertPath);
	auto fragCode = readFile(fragPath);

	//
	// Vulkan instance
	//

	// check layer support
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : validationLayers) {
		bool layerFound = false;
		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			throw std::runtime_error("validation layers requested, but not available");
		}
	}

	// get required extensions
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vulkan Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.pNext = nullptr;
	// Applications that wish to enumerate portability drivers must
	// set the VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit
	// in the VkInstanceCreateInfo flags and enable the
	// VK_KHR_portability_enumeration instance extension.
	instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance");
	}

	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

	// std::cout << "VkLayerProperties " << layerCount << std::endl;
	// std::cout << "available extensions:" << std::endl;
	// std::unordered_set<std::string> available;
	// for (const auto &extension : availableExtensions) {
	// 	std::cout << "\t" << extension.extensionName << std::endl;
	// 	available.insert(extension.extensionName);
	// }
	// std::cout << "required extensions:" << std::endl;
	// for (const auto &required : extensions) {
	// 	std::cout << "\t" << required << std::endl;
	// 	if (available.find(required) == available.end()) {
	// 		throw std::runtime_error("Missing required glfw extension");
	// 	}
	// }

	//
	// physical device
	//

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	std::cout << "Device count: " << deviceCount << std::endl;
	switch (deviceCount) {
		case 0:
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
			break;
		case 1:
			physicalDevice = devices[0];
			break;
		default:
			physicalDevice = chooseBestDevice(devices);
	}

	// logical device
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	// if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
	// 	throw std::runtime_error("failed to create logical device!");
	// }
}

Engine::~Engine() {
	// vulkan
	vkDestroyInstance(instance, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);

	// glfw
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Engine::update() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

QueueFamilyIndices Engine::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		i++;
	}
  return indices;
}

VkPhysicalDevice Engine::chooseBestDevice(std::vector<VkPhysicalDevice> devices) {
	// todo
	return devices[0];
}

std::vector<char> Engine::readFile(const std::string &filepath) {
	std::ifstream file{filepath, std::ios::ate | std::ios::binary};
	if (!file.is_open()) {}
	size_t filesize = (size_t)file.tellg();
	std::vector<char> buffer(filesize);
	file.seekg(0);
	file.read(buffer.data(), filesize);
	file.close();
	return buffer;
}
