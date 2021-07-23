#pragma once

class Devices;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <set>
#include <optional>
#include <utility>



class Devices
{

	// structs
public:

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamilyIndex;
		std::optional<uint32_t> presentFamilyIndex;

		bool isComplete();
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	// public members
public:

	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	QueueFamilyIndices deviceQueueFamilyIndices;
	SwapChainSupportDetails swapChainSupport;

	VkQueue graphicsQueue;
	VkQueue presentQueue;


	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


	// public static functions
public:
	
	static SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice, VkSurfaceKHR);



	// public functions
public:

	void pickPhysicalDevice(VkInstance, VkSurfaceKHR);
	void createLogicalDevice(bool enableValidationLayers, std::vector<const char*> validationLayers);

	void destroyDevice();

	
	//private functions
private:

	std::pair<bool, QueueFamilyIndices> isDeviceSuitable(VkPhysicalDevice, VkSurfaceKHR);

	QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice, VkSurfaceKHR);
	bool checkDeviceExtensionSupport(VkPhysicalDevice);


};


