#include "GraphicsEngine.h"


// Static Functions

VkBool32 GraphicsEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}


// Public Functions ---------------------------

void GraphicsEngine::initializeEngine(std::string appName) {

	glfwInit();
	initializeWindow(appName);
	createVulkanInstance(appName);
	if (enableValidationLayers) { setupDebugMessenger(); }
	initializeSurface();

	setupDevices();

	createSwapChain();
	createImageViews();
	createRenderPass();

	createFramebuffers();
	createCommandPool();
	createCommandBuffers();

	createSyncObjects();
}


void GraphicsEngine::closeEngine() {

	for (size_t i = 0; i < maxFramesInFlight; i++) {
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}
	vkFreeCommandBuffers(logicalDevice, commandPool, (uint32_t)commandBuffers.size(), commandBuffers.data());
	
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	vkDestroyDevice(logicalDevice, nullptr);
	if (enableValidationLayers) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(vulkanInstance, debugMessenger, nullptr);
		}
	}

	vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
	vkDestroyInstance(vulkanInstance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}


bool GraphicsEngine::windowShouldClose() {
	return glfwWindowShouldClose(window);
}

void GraphicsEngine::waitIdle() {
	vkDeviceWaitIdle(logicalDevice);
}

// Private Functions --------------------------


void GraphicsEngine::initializeWindow(std::string appName) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(600, 400, appName.c_str(), NULL, nullptr);

	glfwSetWindowUserPointer(window, this);
}

void GraphicsEngine::createVulkanInstance(std::string appName) {
	if (enableValidationLayers) {
		checkValidationLayerSupport();
	}

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
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
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

void GraphicsEngine::initializeSurface() {
	if (glfwCreateWindowSurface(vulkanInstance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}


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


void GraphicsEngine::createObjects(std::vector<GraphicalObject*>& objects) {
	GraphicalObject::createPipelines(logicalDevice, renderPass, swapChainExtent, objects);
	
	for (size_t i = 0; i < objects.size(); i++) {
		createVertexBuffer(objects[i]);
		createIndexBuffer(objects[i]);
	}
	
	requireRecordCommandBuffers = true;
}

void GraphicsEngine::destroyObjects(std::vector<GraphicalObject*>& objects) {
	for (GraphicalObject* obj : objects) {
		vkDestroyBuffer(logicalDevice, obj->vertexBuffer, nullptr);
		vkDestroyBuffer(logicalDevice, obj->indexBuffer, nullptr);
		vkFreeMemory(logicalDevice, obj->vertexBufferMemory, nullptr);
		vkFreeMemory(logicalDevice, obj->indexBufferMemory, nullptr);

		obj->destroyPipeline(logicalDevice);
	}
	requireRecordCommandBuffers = true;
}

void GraphicsEngine::updateObjects(std::vector<GraphicalObject*>& objects) {
	for (GraphicalObject* obj : objects) {
		vkDestroyBuffer(logicalDevice, obj->vertexBuffer, nullptr);
		vkDestroyBuffer(logicalDevice, obj->indexBuffer, nullptr);
		vkFreeMemory(logicalDevice, obj->vertexBufferMemory, nullptr);
		vkFreeMemory(logicalDevice, obj->indexBufferMemory, nullptr);

		createVertexBuffer(obj);
		createIndexBuffer(obj);
	}
	requireRecordCommandBuffers = true;
}




// Helper Sub-Functions

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






