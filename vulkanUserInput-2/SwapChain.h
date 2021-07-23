#pragma once

class SwapChain;

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>
#include <vector>

#include "Devices.h"
#include "EngineWindow.h"


class SwapChain
{


	// public members
public:

	VkSwapchainKHR chain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> imageViews;

	VkFormat imageFormat;
	VkExtent2D extent;


	// public functions
public:

	void createSwapChain(Devices&, EngineWindow&);
	void destroySwapChain(Devices&);

	

	// private functions
private:

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>);
	VkExtent2D chooseSwapExtent(GLFWwindow*, const VkSurfaceCapabilitiesKHR&);

	void createImageViews(Devices&);


};


