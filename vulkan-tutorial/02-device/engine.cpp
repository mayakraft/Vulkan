#include "engine.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>

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
	window = glfwCreateWindow(512, 512, appName, nullptr, nullptr);

	//
	// Vulkan instance
	//
	// the Vulkan instance is a connection between our app and the Vulkan library
	//

	// we need to tell the instance some information about our app.
	// however, pretty much all of this info is optional.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vulkan Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// to initialize Vulkan, we need to specify which extensions to use,
	// because Vulkan is platform agnostic, these extensions inform Vulkan
	// about our window system specifically, in our case, GLFW.
	// GLFW actually provides a helper function to generate these extensions.

	// Ask GLFW to generate a list of extensions we will need for our app.
	// on my MacOS M1, this results in two: VK_KHR_surface, VK_EXT_metal_surface
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	// see appendex [1]
	instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	// here is the struct we will pass into the create instance function
	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	// we will come back to this layer count later.
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	// create the Vulkan instance. this concludes this section
	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance");
	}

	//
	// Vulkan physical device
	//
	// The physical device refers to the IRL physical hardware.
	// There may be multiple physical devices, in which case, we will want to
	// query the capabilities of each device.
	//

	// right from the start, we are able to query for all Vulkan-enabled devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	// if there are more than one device, we need to somehow only choose 1.
	// we will need to write some kind of heuristic method to determine
	// the best option, or ask the user which they prefer.
	physicalDevice = chooseBestDevice(devices);

	//
	// Vulkan logical device
	//
	// The logical device is a digital interface to a physical device,
	// where we can setup which features we would like to interface with.
	//

	// different queues have the ability to perform different tasks like
	// graphics, encoding/decoding, computing, etc..
	// we need to get the graphics queue from our device, but before we search
	// for it, we have to know how many queues we iterate over.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		queueFamilies.data());

	// iterate over the queues to find which is the graphics queue.
	int graphicsFamily = -1;
	for (int i = 0; i < queueFamilies.size(); i += 1) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
		}
	}

	// this process should be inside the "choose best device" functionality.
	if (graphicsFamily == -1) {
		throw std::runtime_error("selected GPU does not support graphics family!");
	}

	// This struct specifies how many queues we want for this particular
	// queue family, in this case, the graphics queue family.
	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = (uint32_t)graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// Device features include things like geometry shaders, we can require
	// these things here, as long as we detected them in the chooseBestDevice.
	VkPhysicalDeviceFeatures deviceFeatures{};

	// When we create the device, provide this struct.
	// Link the previous two structs, with count info, and set all others to 0.
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.enabledLayerCount = 0;

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	// The Queue family stuff was previously calculated on the physical device.
	// Now that we have a logical device, we can get the actual graphics queue.
	// 0 is the index of the first (and only) queue, if we were creating
	// more than one we would increment the number here.
	vkGetDeviceQueue(logicalDevice, graphicsFamily, 0, &graphicsQueue);

	printInfo(instanceExtensions, deviceCount);
}

/**
 * Destroy and cleanup memory
 */
Engine::~Engine() {
	// vulkan
	vkDestroyDevice(logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);

	// glfw
	glfwDestroyWindow(window);
	glfwTerminate();
}

/**
 * Draw loop
 */
void Engine::startLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

/**
 * Of the (potentially) many Vulkan-ready devices, we need to choose
 * the best fit device for our purposes, so, this is a heuristic
 * method, in which the needs may change from app to app.
 */
VkPhysicalDevice Engine::chooseBestDevice(std::vector<VkPhysicalDevice> devices) {
	for (int i = 0; i < devices.size(); i += 1) {
		auto device = devices[i];
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		// we don't want "other" or "cpu", anything else is okay.
		// if you want to require things, like fullDrawIndexUint32, list them here
		if (true
			// deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER
			// deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU
			&& (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
				|| deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				|| deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
			// && deviceFeatures.robustBufferAccess
			// && deviceFeatures.shaderFloat64
			// && deviceFeatures.fullDrawIndexUint32
			) {
			return device;
		}
	}
	throw std::runtime_error("No suitable Vulkan device found");
}

/**
 * Debug. Print information relevant to Vulkan initialization.
 */
void Engine::printInfo(std::vector<const char *> instanceExtensions, int deviceCount) {
	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());
	std::cout << "available instanceExtensions:" << std::endl;
	std::unordered_set<std::string> available;
	for (const auto &extension : availableExtensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
		available.insert(extension.extensionName);
	}
	std::cout << "required instanceExtensions:" << std::endl;
	for (const auto &required : instanceExtensions) {
		std::cout << "\t" << required << std::endl;
		if (available.find(required) == available.end()) {
			throw std::runtime_error("Missing required glfw extension");
		}
	}
	std::cout << "Device count: " << deviceCount << std::endl;
}

// Appendix
//
// [1]: VK_KHR_portability_enumeration stuff (MacOS specific)
//   there seems to be an issue with MacOS, we have to enable one thing, in two
//   separate places, inside the VkInstanceCreateInfo at properties:
//   - flags VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
//   - ppEnabledExtensionNames VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
//   the warning message that I got was:
//   "Applications that wish to enumerate portability drivers must
//   set the VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit
//   in the VkInstanceCreateInfo flags and enable the
//   VK_KHR_portability_enumeration instance extension."
//   see https://stackoverflow.com/questions/72374316/validation-error-on-device-extension-on-m1-mac
