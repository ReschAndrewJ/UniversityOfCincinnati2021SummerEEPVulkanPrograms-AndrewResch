#include "GraphicsEngine.h"


void GraphicsEngine::shutDown() {
	std::lock_guard<std::mutex> removelock(passData->gObjectsRemoveMutex);

	if (passData->graphicsObjectRemoveVector.size() > 0) {
		std::vector<GraphicsObject*> gObjsToDestroy{};
		for (void* obj : passData->graphicsObjectRemoveVector) {
			gObjsToDestroy.push_back((GraphicsObject*)obj);
		}

		destroyGraphicsObjects(gObjsToDestroy);
	}

	destroyStagingBuffer();

	for (size_t i = 0; i < maxFramesInFlight; i++) {
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}
	vkFreeCommandBuffers(logicalDevice, drawCommandPool, (uint32_t)drawCommandBuffers.size(), drawCommandBuffers.data());

	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	vkDestroyCommandPool(logicalDevice, drawCommandPool, nullptr);

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