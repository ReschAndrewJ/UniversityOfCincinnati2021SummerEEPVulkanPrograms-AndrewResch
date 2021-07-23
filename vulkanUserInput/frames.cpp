#include "engine.h"


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


void engine::processFrame() {

	if (inputHandle.inputs[inputActions::leftMouseJustPressed]) {
		double xPos, yPos;
		glfwGetCursorPos(window.window, &xPos, &yPos);
	
		rect = Rectangle({ xPos, yPos }, { xPos, yPos });
		Rectangle normalizedRect = rect;
		Shape::normalizeCoordinates(normalizedRect, swapChain.extent.width, swapChain.extent.height);
		addObject(normalizedRect.convertToObject());
	}

	if (inputHandle.inputs[inputActions::leftMouseHeld]) {
		double xPos, yPos;
		glfwGetCursorPos(window.window, &xPos, &yPos);

		if (xPos != rect.points[1].x || yPos != rect.points[1].y) {
			rect.points[1] = { xPos, yPos };
			Rectangle normalizedRect = rect;
			Shape::normalizeCoordinates(normalizedRect, swapChain.extent.width, swapChain.extent.height);

			objectUpdateQueue.push_back(std::pair<int, Object>{objects.size() - 1, normalizedRect.convertToObject()});

			std::cout << "{ (" << rect.points[0].x << "," << rect.points[0].y << "),"
				<< "(" << rect.points[1].x << "," << rect.points[1].y << ") }" << std::endl;
		
		}
	}

	if (inputHandle.inputs[inputActions::leftMouseJustReleased]) {
		objectRemoveQueue.push_back(objects.size() - 1);
	}

	



	// clear one frame inputs

	inputHandle.inputs[inputActions::leftMouseJustPressed] = false;
	inputHandle.inputs[inputActions::leftMouseJustReleased] = false;
}