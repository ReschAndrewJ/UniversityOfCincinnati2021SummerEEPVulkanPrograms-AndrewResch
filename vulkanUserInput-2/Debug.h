#pragma once

class Debug;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>


class Debug
{


	// public members
public:

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif // DEBUG



	VkDebugUtilsMessengerEXT debugMessenger;


	// public functions
public:

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);


	// throws error if not available
	void checkRequiredValidationLayersSupport(); 


	void bindValidationLayers(VkInstanceCreateInfo&, VkDebugUtilsMessengerCreateInfoEXT&);
	void setupDebugMessenger(VkInstance);
	void deInstanceDebugMessenger(VkInstance);

	

	// private functions
private:

	void populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
	

	

};


