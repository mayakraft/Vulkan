#include "Device.h"
#include <stdexcept>
#include <iostream>

// The physical device refers to the IRL physical hardware.
// There may be multiple physical devices, in which case, we will want to
// query the capabilities of each device.
Device::Device(GLFWwindow* window, const char* applicationName, const char* engineName) : window(window) {
  createInstance(applicationName, engineName);
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
}

Device::~Device() {
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
}

void Device::createInstance(const char* applicationName, const char* engineName) {
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers required, but not available");
  }

	// we need to tell the instance some information about our app.
	// however, pretty much all of this info is optional.
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = applicationName;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = engineName;
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // to initialize Vulkan, we need to specify which extensions to use,
	// because Vulkan is platform agnostic, these extensions inform Vulkan
	// about our window system specifically, in our case, GLFW.
	// GLFW actually provides a helper function to generate these extensions.

	// Ask GLFW to generate a list of extensions we will need for our app.
	// on my MacOS M1, this results in two: VK_KHR_surface, VK_EXT_metal_surface
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  // extensions.push_back("VK_KHR_portability_subset");
  // see appendex [1]
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  /*extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);*/

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();
  // validation layers for debugging
  if (enableValidationLayers) {
	  createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
	  createInfo.enabledLayerCount = 0;
  }
	createInfo.pNext = nullptr;
  createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  // create the Vulkan instance. this concludes this section
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan instance");
  }
}

void Device::setupDebugMessenger() {
  // no debug messenger for now
}

void Device::createSurface() {
	// To maintain platform agnosticism, Vulkan cannot interface with a window
	// system on its own. Ask GLFW to create this, specific to our platform.
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface");
  }
}

/**
 * Of the (potentially) many Vulkan-ready devices, we need to choose
 * the best fit device for our purposes, so, this is a heuristic
 * method, in which the needs may change from app to app.
 */
// The physical device refers to the IRL physical hardware.
// There may be multiple physical devices, in which case, we will want to
// query the capabilities of each device.
void Device::pickPhysicalDevice() {
	// right from the start, we are able to query for all Vulkan-enabled devices
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// if there are more than one device, we need to somehow only choose 1.
	// we will need to write some kind of heuristic method to determine
	// the best option, or ask the user which they prefer.
  int count = 1;
  for (const auto& deviceCandidate : devices) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(deviceCandidate, &deviceProperties);

    #ifndef NDEBUG
    std::cout << "[INFO] checking a new device (" << count << "/" << devices.size() << "): " << deviceProperties.deviceName << std::endl;
    printAvailableDeviceExtensions(deviceCandidate);
    #endif

    // todo: add check for support for samplerAnisotropy
    // https://vulkan-tutorial.com/en/Texture_mapping/Image_view_and_sampler

		// we don't want "other" or "cpu", anything else is okay.
		// if you want to require things, like fullDrawIndexUint32, list them here
    // deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER
    // deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
			// && deviceFeatures.robustBufferAccess
			// && deviceFeatures.shaderFloat64
			// && deviceFeatures.fullDrawIndexUint32
      // if (checkDeviceExtensionSupport(deviceCandidate)) {
      //   physicalDevice = deviceCandidate;
      //   break;
      // }
      physicalDevice = deviceCandidate;
      msaaSamples = getMaxUsableSampleCount();
      break;
    }

    count++;
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("No suitable Vulkan device found");
  }
}

// The logical device is a digital interface to a physical device,
// where we can setup which features we would like to interface with.
void Device::createLogicalDevice() {
	// different queues have the ability to perform different tasks like
	// graphics, encoding/decoding, computing, etc..
	// we need to get the graphics queue from our device, but before we search
	// for it, we have to know how many queues we iterate over.
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	// iterate over the queues to find which is the graphics queue.
  int graphicsFamily = -1;
  int presentFamily = -1;

  for (int i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

    if (presentSupport) {
      presentFamily = i;
    }
  }

  if (graphicsFamily == -1 || presentFamily == -1) {
    throw std::runtime_error("Selected GPU does not support required queue families");
  }
  // we need to store them to be reused if the swap chain needs to be recreated.
	graphicsQueueFamilyIndex = (uint32_t)graphicsFamily;
	presentQueueFamilyIndex = (uint32_t)presentFamily;

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

	// Device features include things like geometry shaders, we can require
	// these things here, as long as we detected them in the chooseBestDevice.
  VkPhysicalDeviceFeatures deviceFeatures{};
  // added to support anisotropic filtering for textures
  deviceFeatures.samplerAnisotropy = VK_TRUE;
  // multisampling
  deviceFeatures.sampleRateShading = VK_TRUE;

	// When we create the device, provide this struct.
	// Link the previous two structs, with count info, and set all others to 0.
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
  // validation layers for debugging
  if (enableValidationLayers) {
	  deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
	  deviceCreateInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device");
  }

	// The Queue family stuff was previously calculated on the physical device.
	// Now that we have a logical device, we can get the actual graphics queue.
	// 0 is the index of the first (and only) queue, if we were creating
	// more than one we would increment the number here.
  vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
  vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
}

VkSampleCountFlagBits Device::getMaxUsableSampleCount() {
  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

  VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts
    & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
  if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
  if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
  if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
  if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
  if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
  if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
  return VK_SAMPLE_COUNT_1_BIT;
}

bool Device::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layerName : validationLayers) {
    bool layerFound = false;
    for (const auto& layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) {
      return false;
    }
  }
  return true;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

  for (const auto& extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

void Device::printAvailableDeviceExtensions(VkPhysicalDevice device) {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  if (extensionCount == 0) {
    std::cout << "[INFO] no device extensions on this device" << std::endl;
    return;
  }

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

  std::cout << "[INFO] available device extensions:" << std::endl;
  for (const auto& extension : extensions) {
    std::cout << "  - " << extension.extensionName << std::endl;
  }
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
