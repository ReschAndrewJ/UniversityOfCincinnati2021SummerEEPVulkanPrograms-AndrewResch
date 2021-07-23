#include "Debug.h"

#include <iostream>



// public functions -----------------------------------------


VkBool32 Debug::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;

}


void Debug::bindValidationLayers(VkInstanceCreateInfo& vkCreateInfo, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo) {
	vkCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	vkCreateInfo.ppEnabledLayerNames = validationLayers.data();

	
	populateMessengerCreateInfo(debugCreateInfo);
	vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
}


void Debug::setupDebugMessenger(VkInstance vulkanInstance) {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
	populateMessengerCreateInfo(messengerCreateInfo);


	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vulkanInstance, &messengerCreateInfo, nullptr, &debugMessenger);
	} else {
		throw std::runtime_error("failed to set up debug messenger");
	}

}


void Debug::deInstanceDebugMessenger(VkInstance vulkanInstance) {
	if (!enableValidationLayers) return;

	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vulkanInstance, debugMessenger, nullptr);
	}

}




void Debug::checkRequiredValidationLayersSupport() {
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
			throw std::runtime_error("validation Layers not avaliable");
		}
	}

	
}



// private functions ----------------------------------------


void Debug::populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& messengerCreateInfo) {
	
	messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messengerCreateInfo.pfnUserCallback = debugCallback;

}

