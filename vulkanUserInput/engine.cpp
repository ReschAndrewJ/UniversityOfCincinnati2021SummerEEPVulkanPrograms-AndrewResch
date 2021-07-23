#include "engine.h"

// public functions -----------------------------------------

void engine::initializeEngine() {
	glfwInit();
	window.initWindow();
	setupEnginePointer();

	createVulkanInstance();
	debugMessenger.setupDebugMessenger(vulkanInstance);
	window.createSurface(vulkanInstance);

	devices.pickPhysicalDevice(vulkanInstance, window.surface);
	devices.createLogicalDevice(debugMessenger.enableValidationLayers, debugMessenger.validationLayers);

	swapChain.createSwapChain(devices, window);
	pipeline.createRenderPass(devices.logicalDevice, swapChain.imageFormat);

	buffers.createFrameBuffers(devices.logicalDevice, swapChain, pipeline);
	buffers.createCommandPool(devices);
	buffers.createCommandBuffers(devices, swapChain, pipeline, objects);
	
	sync.createSyncObjects(devices.logicalDevice, swapChain.swapChainImages);

	setupInput();

}


void engine::closeEngine() {

	for (int i = objects.size() - 1; i >= 0; i--) {
		removeObject(i);
	}

	sync.destroySyncObjects(devices.logicalDevice);

	buffers.destroyFrameBuffers(devices.logicalDevice);
	buffers.destroyCommandBuffers(devices.logicalDevice);

	pipeline.destroyRenderPass(devices.logicalDevice);
	swapChain.destroySwapChain(devices);

	buffers.destroyCommandPool(devices.logicalDevice);
	devices.destroyDevice();

	if (debugMessenger.enableValidationLayers) {
		debugMessenger.deInstanceDebugMessenger(vulkanInstance);
	}
	
	window.endWindow(vulkanInstance);
	glfwTerminate();
}


void engine::addObject(Object obj) {
	pipeline.createGraphicsPipeline(devices.logicalDevice, swapChain.extent);
	
	objects.push_back(obj);
	buffers.createVertexBuffer(devices, swapChain, obj);
	buffers.createIndexBuffer(devices, obj);

	objectsChanged = true;
}

void engine::removeObject(size_t index) {
	buffers.destroyObjectBuffers(devices.logicalDevice, index);
	objects.erase(objects.begin() + index);
	pipeline.destroyPipeline(devices.logicalDevice, index);

	objectsChanged = true;
}

void engine::updateObject(Object obj, int index) {
	objects[index] = obj;
	buffers.destroyObjectBuffers(devices.logicalDevice, index);
	pipeline.destroyPipeline(devices.logicalDevice, index);

	pipeline.createGraphicsPipeline(devices.logicalDevice, swapChain.extent, index);
	buffers.createVertexBuffer(devices, swapChain, obj, index);
	buffers.createIndexBuffer(devices, obj, index);

	objectsChanged = true;
}


void engine::mainLoop() {
	while (!glfwWindowShouldClose(window.window)) {
		glfwPollEvents();

		for (int i = objectUpdateQueue.size() - 1; i >= 0; i--) {
			updateObject(objectUpdateQueue[i].second, objectUpdateQueue[i].first);
		}
		objectUpdateQueue.clear();

		std::sort(objectRemoveQueue.begin(), objectRemoveQueue.end());
		for (int i = objectRemoveQueue.size() - 1; i >= 0; i--) {
			removeObject(i);
		}
		objectRemoveQueue.clear();

		if (objectsChanged) {
			objectsChanged = false;
			vkDeviceWaitIdle(devices.logicalDevice);
			buffers.recordCommandBuffers(swapChain, pipeline, objects);
		}

		drawFrame();
		processFrame();
	
		vkDeviceWaitIdle(devices.logicalDevice);
	}
	
	
}


void engine::frameBufferResizedCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<engine*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}


// private functions ----------------------------------------

void engine::createVulkanInstance() {
	if (debugMessenger.enableValidationLayers) {
		debugMessenger.checkRequiredValidationLayersSupport();
	}

	VkApplicationInfo vkAppInfo{};
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pApplicationName = "Sample";
	vkAppInfo.applicationVersion = VK_API_VERSION_1_2;
	vkAppInfo.pEngineName = "No Engine";
	vkAppInfo.engineVersion = VK_API_VERSION_1_2;
	vkAppInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo vulkanInstanceCreateInfo{};
	vulkanInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vulkanInstanceCreateInfo.pApplicationInfo = &vkAppInfo;

	auto extensions = getRequiredExtensions();
	vulkanInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	vulkanInstanceCreateInfo.ppEnabledExtensionNames = extensions.data();
	
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (debugMessenger.enableValidationLayers) {
		debugMessenger.bindValidationLayers(vulkanInstanceCreateInfo, debugCreateInfo);
	}
	else {
		vulkanInstanceCreateInfo.enabledLayerCount = 0;
		vulkanInstanceCreateInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&vulkanInstanceCreateInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance");
	}

}


void engine::setupEnginePointer() {
	glfwSetWindowUserPointer(window.window, this);
	glfwSetFramebufferSizeCallback(window.window, frameBufferResizedCallback);
}


std::vector<const char*> engine::getRequiredExtensions() {
	uint32_t glfwextensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwextensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwextensionCount);

	if (debugMessenger.enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}




void engine::rebuildSwapChain() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(window.window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(devices.logicalDevice);

	buffers.destroyFrameBuffers(devices.logicalDevice);
	buffers.destroyCommandBuffers(devices.logicalDevice);

	int pipelineCount = pipeline.pipelines.size();
	
	for (int i = pipelineCount - 1; i >= 0; i--) {
		pipeline.destroyPipeline(devices.logicalDevice, i);
	}
	pipeline.destroyRenderPass(devices.logicalDevice);
	swapChain.destroySwapChain(devices);

	swapChain.createSwapChain(devices, window);
	pipeline.createRenderPass(devices.logicalDevice, swapChain.imageFormat);
	for (int i = 0; i < pipelineCount; i++) {
		pipeline.createGraphicsPipeline(devices.logicalDevice, swapChain.extent);
	}
	buffers.createFrameBuffers(devices.logicalDevice, swapChain, pipeline);
	buffers.createCommandBuffers(devices, swapChain, pipeline, objects);

	sync.imagesInFlight.resize(swapChain.swapChainImages.size(), VK_NULL_HANDLE);
}



void engine::setupInput() {
	glfwSetMouseButtonCallback(window.window, mouseButtonCallback);
}


// callback functions ----------------------------------------

void engine::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	engine* app = reinterpret_cast<engine*>(glfwGetWindowUserPointer(window));

	app->inputHandle.mouseButtonAction(button, action);
}

