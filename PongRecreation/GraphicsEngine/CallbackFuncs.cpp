#include "GraphicsEngine.h"


VkBool32 GraphicsEngine::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}


void GraphicsEngine::framebufferResizedCallback(GraphicsEngine* engine) {
	engine->framebufferResized = true;
}