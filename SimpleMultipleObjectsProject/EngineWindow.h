#pragma once

class EngineWindow;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>


class EngineWindow
{


	// public members
public:

	GLFWwindow* window;
	VkSurfaceKHR surface;

	

	// public functions
public:

	


	void initWindow(int height = 600, int width = 600) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, "Sample", nullptr, nullptr);
	}


	void createSurface(VkInstance vulkanInstance) {
		if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}


	void endWindow(VkInstance vulkanInstance) {
		vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
		vkDestroyInstance(vulkanInstance, nullptr);
		glfwDestroyWindow(window);
	}


};


