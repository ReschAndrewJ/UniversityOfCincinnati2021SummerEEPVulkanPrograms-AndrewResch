#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <optional>

#include "GraphicalObject.h"

class GraphicsEngine
{


	
private:

	// Private Constants

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


	// Structs

	struct QueueFamiliyIndices {
		std::optional<uint32_t> graphicsFamilyIndex;
		std::optional<uint32_t> presentFamilyIndex;
		bool isComplete();
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	// Private Members

	GLFWwindow* window;
	VkSurfaceKHR surface;

	VkInstance vulkanInstance;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	QueueFamiliyIndices deviceQueueFamilyIndices;
	SwapChainSupportDetails swapChainSupport;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandPool commandPool;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	
	size_t maxFramesInFlight = 2;
	size_t currentFrame = 0;

	bool framebufferResized;
	bool requireRecordCommandBuffers;

public:

	// Static Functions

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	static void framebufferResizedCallback(GLFWwindow*, int, int);

	// Public Functions

	// initializeEngine(string appName)
	void initializeEngine(std::string);

	void closeEngine();

	void drawFrame(std::vector<GraphicalObject*>&);
	void recordCommandBuffers(const std::vector<GraphicalObject*>&);

	void createObjects(std::vector<GraphicalObject*>&);
	void destroyObjects(std::vector<GraphicalObject*>&);
	void updateObjects(std::vector<GraphicalObject*>&);

	bool windowShouldClose();
	void waitIdle();

private:

	// Private Functions

	void initializeWindow(std::string);
	void createVulkanInstance(std::string);
	void setupDebugMessenger();
	void initializeSurface();
	void setupDevices();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
	void setupCallbacks();

	void rebuildSwapChain(std::vector<GraphicalObject*>&);


	// Helper Sub-Functions

	void checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);

	bool isDeviceSuitable(VkPhysicalDevice);
	QueueFamiliyIndices findQueueFamilyIndices(VkPhysicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat();
	VkPresentModeKHR chooseSwapPresentMode();
	VkExtent2D chooseSwapExtent();

	void createIndexBuffer(GraphicalObject*);
	void createVertexBuffer(GraphicalObject*);

	void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags,
		VkBuffer&, VkDeviceMemory&);
	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
	uint32_t findMemoryType(uint32_t, VkMemoryPropertyFlags);

};

