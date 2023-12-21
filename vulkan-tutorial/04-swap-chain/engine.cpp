#include "engine.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <cstdint>
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
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

	// shaders
	auto vertCode = readFile(vertPath);
	auto fragCode = readFile(fragPath);

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
	// window surface
	//
	// To maintain platform agnosticism, Vulkan cannot interface with a window
	// system on its own. Ask GLFW to create this, specific to our platform.
	//

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
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
		throw std::runtime_error("failed to find GPUs with Vulkan support");
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
	int presentFamily = -1;
	for (int i = 0; i < queueFamilies.size(); i += 1) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (presentSupport) {
			presentFamily = i;
		}
	}

	// this process should be inside the "choose best device" functionality.
	if (graphicsFamily == -1 || presentFamily == -1) {
		throw std::runtime_error("selected GPU does not support correct queue families");
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		(uint32_t)graphicsFamily,
		(uint32_t)presentFamily
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// // This struct specifies how many queues we want for this particular
	// // queue family, in this case, the graphics queue family.
	// float queuePriority = 1.0f;
	// VkDeviceQueueCreateInfo queueCreateInfo{};
	// queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	// queueCreateInfo.queueFamilyIndex = (uint32_t)graphicsFamily;
	// queueCreateInfo.queueCount = 1;
	// queueCreateInfo.pQueuePriorities = &queuePriority;

	// Device features include things like geometry shaders, we can require
	// these things here, as long as we detected them in the chooseBestDevice.
	VkPhysicalDeviceFeatures deviceFeatures{};

	// When we create the device, provide this struct.
	// Link the previous two structs, with count info, and set all others to 0.
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device");
	}

	// The Queue family stuff was previously calculated on the physical device.
	// Now that we have a logical device, we can get the actual graphics queue.
	// 0 is the index of the first (and only) queue, if we were creating
	// more than one we would increment the number here.
	vkGetDeviceQueue(logicalDevice, graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, presentFamily, 0, &presentQueue);

	//
	// Swap Chain
	//

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

	std::vector<VkSurfaceFormatKHR> formats;
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	}

	std::vector<VkPresentModeKHR> presentModes;
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	}

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes);
	VkExtent2D extent = chooseSwapExtent(capabilities);
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0
		&& imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	std::cout << "formatCount " << formatCount << std::endl;
	std::cout << "presentModeCount " << presentModeCount << std::endl;
	std::cout << "capabilities.currentExtent " << capabilities.currentExtent.width << " " << capabilities.currentExtent.height << std::endl;
	std::cout << "surfaceFormat.format " << surfaceFormat.format << std::endl;
	std::cout << "imageCount " << imageCount << std::endl;

	uint32_t swapQueueFamilyIndices[] = {
		(uint32_t)graphicsFamily,
		(uint32_t)presentFamily
	};
	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// if (indices.graphicsFamily != indices.presentFamily) {
	// 	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	// 	swapCreateInfo.queueFamilyIndexCount = 2;
	// 	swapCreateInfo.pQueueFamilyIndices = swapQueueFamilyIndices;
	// } else {
	// 	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// 	swapCreateInfo.queueFamilyIndexCount = 0; // Optional
	// 	swapCreateInfo.pQueueFamilyIndices = nullptr; // Optional
	// }
	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	swapCreateInfo.queueFamilyIndexCount = 2;
	swapCreateInfo.pQueueFamilyIndices = swapQueueFamilyIndices;
	swapCreateInfo.preTransform = capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &swapCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	//
	// Swap Chain Image Views
	//

	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
	printInfo(instanceExtensions, deviceCount);
}

/**
 * Destroy and cleanup memory
 */
Engine::~Engine() {
	// vulkan
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
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

VkSurfaceFormatKHR Engine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR Engine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        std::cout << "actualExtent " << width << " " << height << std::endl;

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
/**
 * Useful for opening the compiled shader binary files.
 */
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
