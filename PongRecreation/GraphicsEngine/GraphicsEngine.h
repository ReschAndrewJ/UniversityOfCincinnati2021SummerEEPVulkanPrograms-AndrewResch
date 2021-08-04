#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <list>
#include <optional>
#include <algorithm>

#include "../Objects/GraphicsObject.h"

#include "../ApplicationEngine/PassInfo.h"

class GraphicsEngine
{

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
#endif // NDEBUG

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamilyIndex;
		uint32_t graphicsFamilyQueueCount = 0;
		std::optional<uint32_t> presentFamilyIndex;
		bool isComplete() {
			return graphicsFamilyIndex.has_value()
				&& presentFamilyIndex.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	size_t maxFramesInFlight = 2;
	size_t currentFrame = 0;
	bool framebufferResized;
	VkDeviceSize bufferAllocationSize = (VkDeviceSize)pow(2, 10);

	VkDebugUtilsMessengerEXT debugMessenger;

	GLFWwindow* window;
	VkSurfaceKHR surface;

	VkInstance vulkanInstance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue helperQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

	std::vector<VkCommandBuffer> drawCommandBuffers;
	VkCommandPool drawCommandPool;
	VkCommandPool helperCommandPool;
	
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	
	QueueFamilyIndices queryDeviceQueueFamilyIndices(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	std::vector<VkBuffer> objectsBuffers;
	std::vector<VkDeviceMemory> objectsBuffersMemories;
	std::vector<std::set<SharedStructs::bufferSpace>> objectsBuffersInfos;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	std::set<SharedStructs::bufferSpace> stagingBufferInfo;

	
	std::vector<GraphicsObject*> drawObjects{};
	
	PassInfo* passData;

public:

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pcallbackData,
		void* pUserData);

	static void framebufferResizedCallback(GraphicsEngine* engine);


	GLFWwindow* getWindowPointer() { return window; }
	void waitIdle() { vkDeviceWaitIdle(logicalDevice); }

	void startUp(std::string appName, int width, int height, 
		void* appEnginePointer, PassInfo* sharedPass);
	void shutDown();

	void graphicsFrame();
	void createDestroyGraphicsObjects();
	

private:

	void createVulkanInstance(std::string appName);

	void checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& messengerCreateInfo);

	void setupDevices();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void createSwapChain();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(SwapChainSupportDetails support);
	VkPresentModeKHR chooseSwapPresentMode(SwapChainSupportDetails support);
	VkExtent2D chooseSwapExtent(SwapChainSupportDetails support);

	void createImageViews();
	void createRenderPass();

	void createFramebuffers();

	void createCommandPools();

	void createDrawCommandBuffers();

	void createSyncObjects();

	void createStagingBuffer();
	void createObjectsBuffer();
	void createBuffer(VkBuffer& buffer, VkDeviceMemory& memory,
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	void destroyObjectsBuffer(size_t index);
	void destroyStagingBuffer();

	void flushStagingBuffer();
	
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createGraphicsObjects(std::vector<GraphicsObject*>& newObjects);
	bool stageAllocateObject(GraphicsObject* object);
	

	void destroyGraphicsObjects(std::vector<GraphicsObject*>& objectsToDestroy);


	void rebuildSwapChain(std::vector<GraphicsObject*>& objects);

	void recordCommandBuffers(const std::vector<GraphicsObject*>& objects);

};