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


	void initWindow(int height = 600, int width = 600);
	void createSurface(VkInstance);
	void endWindow(VkInstance);



};



