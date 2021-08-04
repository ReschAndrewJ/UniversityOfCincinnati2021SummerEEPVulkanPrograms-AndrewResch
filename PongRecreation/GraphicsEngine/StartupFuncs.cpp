#include "GraphicsEngine.h"



void GraphicsEngine::startUp(std::string appName, int width,
	int height, void* appEnginePointer, PassInfo* sharedPass) {

	passData = sharedPass;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(width, height, appName.c_str(), NULL, nullptr);
	glfwSetWindowUserPointer(window, appEnginePointer);

	createVulkanInstance(appName);
	if (enableValidationLayers) { setupDebugMessenger(); };

	if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}

	setupDevices();

	createSwapChain();
	createImageViews();
	createRenderPass();

	createFramebuffers();
	createCommandPools();
	createDrawCommandBuffers();

	createSyncObjects();

	createStagingBuffer();
}



// -------------------------------------

void GraphicsEngine::createVulkanInstance(std::string appName) {
	if (enableValidationLayers) { checkValidationLayerSupport(); }

	VkApplicationInfo vkAppInfo{};
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pApplicationName = appName.c_str();
	vkAppInfo.applicationVersion = VK_API_VERSION_1_2;
	vkAppInfo.pEngineName = "No Engine";
	vkAppInfo.engineVersion = VK_API_VERSION_1_2;
	vkAppInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &vkAppInfo;

	auto extensions = getRequiredExtensions();
	instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		instanceCreateInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vulkan instance");
	}
}


void GraphicsEngine::checkValidationLayerSupport() {
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
		}}
		if (!layerFound) {
			throw std::runtime_error("validation layers not available");
}}}


std::vector<const char*> GraphicsEngine::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}


// -----------------------------------------------------------


void GraphicsEngine::setupDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
	populateDebugMessengerCreateInfo(messengerCreateInfo);

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vulkanInstance, &messengerCreateInfo, nullptr, &debugMessenger);
	}
	else {
		throw std::runtime_error("failed to set up debug messenger");
	}
}


void GraphicsEngine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& messengerCreateInfo) {
	messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messengerCreateInfo.messageSeverity = 0
//		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 
//		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	messengerCreateInfo.messageType = 0
		| VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messengerCreateInfo.pfnUserCallback = debugCallback;
}


// ------------------------------------------------


void GraphicsEngine::setupDevices() {

	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan");
	}
	std::vector<VkPhysicalDevice> availableDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &physicalDeviceCount, availableDevices.data());

	QueueFamilyIndices deviceQueueFamilyIndices;
	for (const auto& device : availableDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			deviceQueueFamilyIndices = queryDeviceQueueFamilyIndices(device);
			break;
	}}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU");
	}


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
	if (deviceQueueFamilyIndices.graphicsFamilyQueueCount > 1) {
		vkGetDeviceQueue(logicalDevice, deviceQueueFamilyIndices.graphicsFamilyIndex.value(), 1, &helperQueue);
	}
}


bool GraphicsEngine::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = queryDeviceQueueFamilyIndices(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty()
			&& !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && swapChainAdequate;
}


bool GraphicsEngine::checkDeviceExtensionSupport(VkPhysicalDevice device) {
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


// -------------------------------------------------------


void GraphicsEngine::createSwapChain() {
	SwapChainSupportDetails swapSupport = querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapSupport);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapSupport);
	VkExtent2D extent = chooseSwapExtent(swapSupport);

	uint32_t imageCount = std::clamp(swapSupport.capabilities.minImageCount + 1,
									swapSupport.capabilities.minImageCount, 
									swapSupport.capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	QueueFamilyIndices familyIndices = queryDeviceQueueFamilyIndices(physicalDevice);
	if (familyIndices.graphicsFamilyIndex != familyIndices.presentFamilyIndex) {
		uint32_t queueFamilyIndices[] = {
			familyIndices.graphicsFamilyIndex.value(),
			familyIndices.presentFamilyIndex.value()
		};

		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapCreateInfo.queueFamilyIndexCount = 2;
		swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapCreateInfo.preTransform = swapSupport.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(logicalDevice, &swapCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());
	
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}


VkSurfaceFormatKHR GraphicsEngine::chooseSwapSurfaceFormat(SwapChainSupportDetails support) {
	auto availableFormats = support.formats;
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
	}}

	return availableFormats[0];
}


VkPresentModeKHR GraphicsEngine::chooseSwapPresentMode(SwapChainSupportDetails support) {
	auto availablePresentModes = support.presentModes;
	for (const auto& availableMode : availablePresentModes) {
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availableMode;
	}}

	return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D GraphicsEngine::chooseSwapExtent(SwapChainSupportDetails support) {
	auto capabilities = support.capabilities;
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			(uint32_t)width,
			(uint32_t)height
		};

		actualExtent.width = std::clamp(actualExtent.width, 
										capabilities.minImageExtent.width, 
										capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, 
										capabilities.minImageExtent.height, 
										capabilities.maxImageExtent.height);

		return actualExtent;
	}
}


// -------------------------------------------------------


void GraphicsEngine::createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = swapChainImages[i];
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = swapChainImageFormat;
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &viewCreateInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view");
		}
	}
}


// ------------------------------------------------------


void GraphicsEngine::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass");
	}
}


// -------------------------------------------------------


void GraphicsEngine::createFramebuffers() {
	framebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = { swapChainImageViews[i] };

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = renderPass;
		frameBufferInfo.attachmentCount = 1;
		frameBufferInfo.pAttachments = attachments;
		frameBufferInfo.width = swapChainExtent.width;
		frameBufferInfo.height = swapChainExtent.height;
		frameBufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer");
		}
	}
}


// ------------------------------------------------------


void GraphicsEngine::createCommandPools() {
	QueueFamilyIndices indices = queryDeviceQueueFamilyIndices(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamilyIndex.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &drawCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create draw command pool");
	}

	if (indices.graphicsFamilyQueueCount > 1) {
		poolInfo = VkCommandPoolCreateInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &helperCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("tried and failed to create helper command pool");
		}
	}
	else {
		helperCommandPool = drawCommandPool;
	}
}


// -------------------------------------------------------


void GraphicsEngine::createDrawCommandBuffers() {
	drawCommandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = drawCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)drawCommandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, drawCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}
}


// -------------------------------------------------------


void GraphicsEngine::createSyncObjects() {
	imageAvailableSemaphores.resize(maxFramesInFlight);
	renderFinishedSemaphores.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < maxFramesInFlight; i++) {
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}


// -------------------------------------------------------


GraphicsEngine::QueueFamilyIndices GraphicsEngine::queryDeviceQueueFamilyIndices(VkPhysicalDevice device) {
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
			&& queueFamily.queueCount > indices.graphicsFamilyQueueCount) {
			indices.graphicsFamilyIndex = i;
			indices.graphicsFamilyQueueCount = queueFamily.queueCount;
		}
		
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) { indices.presentFamilyIndex = i; }
		if (indices.isComplete()) { break; }

		i++;
	}

	return indices;
}


GraphicsEngine::SwapChainSupportDetails GraphicsEngine::querySwapChainSupport(VkPhysicalDevice device) {
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