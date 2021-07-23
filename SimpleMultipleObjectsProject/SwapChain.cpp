#include "SwapChain.h"



// public functions -----------------------------------------

void SwapChain::createSwapChain(Devices& devices, EngineWindow& engineWindow) {
	devices.swapChainSupport = Devices::querySwapChainSupportDetails(devices.physicalDevice, engineWindow.surface);
	Devices::SwapChainSupportDetails supportDetails = devices.swapChainSupport;

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(supportDetails.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(supportDetails.presentModes);
	VkExtent2D extent = chooseSwapExtent(engineWindow.window, supportDetails.capabilities);

	uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
	if (supportDetails.capabilities.minImageCount > 0
		&& imageCount > supportDetails.capabilities.maxImageCount) {
		imageCount = supportDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = engineWindow.surface;
	
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (devices.deviceQueueFamilyIndices.graphicsFamilyIndex
		!= devices.deviceQueueFamilyIndices.presentFamilyIndex) {

		uint32_t queueFamilyIndices[] = {
			devices.deviceQueueFamilyIndices.graphicsFamilyIndex.value(),
			devices.deviceQueueFamilyIndices.presentFamilyIndex.value()
		};

		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapCreateInfo.queueFamilyIndexCount = 2;
		swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapCreateInfo.preTransform = supportDetails.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(devices.logicalDevice, &swapCreateInfo, nullptr, &chain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapChain");
	}

	vkGetSwapchainImagesKHR(devices.logicalDevice, chain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(devices.logicalDevice, chain, &imageCount, swapChainImages.data());

	imageFormat = surfaceFormat.format;
	this->extent = extent;

	createImageViews(devices);
}


void SwapChain::destroySwapChain(Devices& devices) {
	for (auto imageView : imageViews) {
		vkDestroyImageView(devices.logicalDevice, imageView, nullptr);
	}

	vkDestroySwapchainKHR(devices.logicalDevice, chain, nullptr);
}


// private functions ----------------------------------------

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			
			return availableFormat;
		}
	}

	return availableFormats[0];
}


VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
	for (const auto& availableMode : availablePresentModes) {
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availableMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D SwapChain::chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		
		return actualExtent;
	}
}


void SwapChain::createImageViews(Devices& devices) {
	imageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size();  i++) {
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = swapChainImages[i];
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = imageFormat;
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(devices.logicalDevice, &viewCreateInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view");
		}
	}

}
