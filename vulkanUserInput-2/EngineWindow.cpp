#include "EngineWindow.h"


void EngineWindow::initWindow(int width, int height) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, "Sample", nullptr, nullptr);
}


void EngineWindow::createSurface(VkInstance vulkanInstance) {
	if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}


void EngineWindow::endWindow(VkInstance vulkanInstance) {
	vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
	vkDestroyInstance(vulkanInstance, nullptr);
	glfwDestroyWindow(window);
}




