#include "Devices.h"


// struct functions -----------------------------------------

bool Devices::QueueFamilyIndices::isComplete() {
	return graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
}


// static public functions ----------------------------------

Devices::SwapChainSupportDetails Devices::querySwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
	SwapChainSupportDetails details;
	uint32_t formatCount;
	uint32_t presentModeCount;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}


// public functions -----------------------------------------

void Devices::pickPhysicalDevice(VkInstance vulkanInstance, VkSurfaceKHR windowSurface) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan Support");
	}

	std::vector<VkPhysicalDevice> availableDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, availableDevices.data());
	
	for (const auto& device : availableDevices) {
		std::pair<bool, QueueFamilyIndices> suitability = isDeviceSuitable(device, windowSurface);
		if (suitability.first) {
			physicalDevice = device;
			deviceQueueFamilyIndices = suitability.second;
			swapChainSupport = querySwapChainSupportDetails(physicalDevice, windowSurface);
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU");
	}
}


void Devices::createLogicalDevice(bool enableValidationLayers, std::vector<const char*> validationLayers) {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		deviceQueueFamilyIndices.graphicsFamilyIndex.value(),
		deviceQueueFamilyIndices.presentFamilyIndex.value()
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

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo logicalDeviceCreateInfo{};
	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	
	if (enableValidationLayers) {
		logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		logicalDeviceCreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device");
	}

	vkGetDeviceQueue(logicalDevice, deviceQueueFamilyIndices.graphicsFamilyIndex.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, deviceQueueFamilyIndices.presentFamilyIndex.value(), 0, &presentQueue);
}


void Devices::destroyDevice() {
	vkDestroyDevice(logicalDevice, nullptr);
}






// private functions ----------------------------------------


std::pair<bool, Devices::QueueFamilyIndices> Devices::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR windowSurface) {
	QueueFamilyIndices indices = findQueueFamilyIndices(device, windowSurface);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupportDetails(device, windowSurface);
		swapChainAdequate = !swapChainSupport.formats.empty() 
			&& !swapChainSupport.presentModes.empty();
	}

	return std::pair<bool, QueueFamilyIndices> {
		indices.isComplete() && swapChainAdequate, 
		indices
	};

}


Devices::QueueFamilyIndices Devices::findQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamilyIndex = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamilyIndex = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}


bool Devices::checkDeviceExtensionSupport(VkPhysicalDevice device) {
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





