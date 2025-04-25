#pragma once

// this one line is required to include the key:
// VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, or "VK_KHR_portability_subset"
// so if we don't include it, or if any problems arise, simply replace the
// macro with the string literal
#define VK_ENABLE_BETA_EXTENSIONS

// tell glfw that we want the additional Vulkan functionality
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
  glm::vec2 position;
  glm::vec3 color;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);
    return attributeDescriptions;
  }
};

class Engine {

public:
	Engine();
	~Engine();
	void startLoop();
	void drawFrame();

  const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}
  };

	bool framebufferResized = false;

private:
	const char *appName = "Triangle";
	const char *vertPath = "./simple.vert.spv";
	const char *fragPath = "./simple.frag.spv";

	const int MAX_FRAMES_IN_FLIGHT = 2;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// The cross-platform window visibile to the user
	GLFWwindow *window;

	// we only need one instance. clean up upon application exit.
	VkInstance instance;

	// surface must be initialized before devices, it can affect device selection
	VkSurfaceKHR surface;

	// physical device will be implicitly cleaned up on its own.
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	// logical devices will need to be cleaned up, before the instance itself
	VkDevice logicalDevice;

	// device queues are implicitly cleaned up on their own.
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;

	// swap chain
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// pipeline
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// frame buffers
	std::vector<VkFramebuffer> swapChainFramebuffers;

	uint32_t currentFrame = 0;

  // buffers
  // cleaned up manually in the cleanup function
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

	// commands
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	// synchronization objects
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	//
	// methods
	//

	// file system. used for reading shader binary files.
	std::vector<char> readFile(const std::string &filepath);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	// heuristic method for choosing one of the possible many Vulkan devices
	VkPhysicalDevice chooseBestDevice(std::vector<VkPhysicalDevice> devices);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// methods encapsulating setup code
	void createSwapChain();
	void createImageViews();
	void createFramebuffers();
	void recreateSwapChain();
	void cleanupSwapChain();

	// debug
	void printInfo(std::vector<const char *> extensions, int deviceCount);
};
