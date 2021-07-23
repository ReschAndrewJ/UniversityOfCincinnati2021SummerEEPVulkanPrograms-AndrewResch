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


void engine::mainLoop() {
	while (!glfwWindowShouldClose(window.window)) {
		glfwPollEvents();
		
		if (objectsChanged) {
			objectsChanged = false;
			vkDeviceWaitIdle(devices.logicalDevice);
			buffers.recordCommandBuffers(swapChain, pipeline, objects);
		}

		drawFrame();
	}

	vkDeviceWaitIdle(devices.logicalDevice);
	
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


void engine::drawFrame() {
	vkWaitForFences(devices.logicalDevice, 1, 
		&sync.inFlightFences[sync.currentFrame], VK_TRUE, UINT64_MAX);
	
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(devices.logicalDevice,
		swapChain.chain, UINT64_MAX, sync.imageAvailableSemaphores[sync.currentFrame],
		VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		rebuildSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to aqcuire swap chain image");
	} 

	if (sync.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(devices.logicalDevice, 1, &sync.imagesInFlight[imageIndex],
						VK_TRUE, UINT64_MAX);
	}
	sync.imagesInFlight[imageIndex] = sync.inFlightFences[sync.currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { sync.imageAvailableSemaphores[sync.currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffers.commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { sync.renderFinishedSemaphores[sync.currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(devices.logicalDevice, 1, &sync.inFlightFences[sync.currentFrame]);

	if (vkQueueSubmit(devices.graphicsQueue, 1, &submitInfo, sync.inFlightFences[sync.currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain.chain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(devices.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		rebuildSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image");
	}

	sync.currentFrame = (sync.currentFrame + 1) & sync.MAX_FRAMES_IN_FLIGHT;
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

