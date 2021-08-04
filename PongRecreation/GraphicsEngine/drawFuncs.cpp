#include "GraphicsEngine.h"





void GraphicsEngine::graphicsFrame() {
	recordCommandBuffers(drawObjects);

	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], 
		VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX,
		imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
		&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		rebuildSwapChain(drawObjects);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(logicalDevice, 1, &imagesInFlight[imageIndex],
			VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &drawCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		rebuildSwapChain(drawObjects);
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image");
	}
	currentFrame = (currentFrame + 1) % maxFramesInFlight;
}



void GraphicsEngine::rebuildSwapChain(std::vector<GraphicsObject*>& objects) {
	std::cout << "rebuilding swap chain" << std::endl;
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice);
	for (size_t i = 0; i < framebuffers.size(); i++) {
		vkDestroyFramebuffer(logicalDevice, framebuffers[i], nullptr);
	}
	vkFreeCommandBuffers(logicalDevice, drawCommandPool,
		(uint32_t)drawCommandBuffers.size(), drawCommandBuffers.data());

	for (GraphicsObject* obj : objects) {
		vkDestroyPipeline(logicalDevice, obj->graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, obj->graphicsPipelineLayout, nullptr);
	}
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

	createSwapChain();
	createImageViews();
	createRenderPass();

	GraphicsObject::createGraphicsPipelines(logicalDevice, renderPass,
		swapChainExtent, objects);

	createFramebuffers();
	createDrawCommandBuffers();

	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
}


void GraphicsEngine::recordCommandBuffers(const std::vector<GraphicsObject*>& objects) {
	for (size_t i = 0; i < drawCommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(drawCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording draw command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[i];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = { 0.1, 0.1, 0.1, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(drawCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		for (GraphicsObject* obj : objects) {
			if (obj->getVertexData(obj->ownerObject).bufferSize == 0
				|| obj->getIndicesData(obj->ownerObject).indexCount == 0) {
				continue;
			}
			/*std::cout << "recording object" << std::endl;
			std::cout << "vertex memory offset: " << obj->vertexMemoryInfo.position << std::endl;
			std::cout << "index memory offset: " << obj->indexMemoryInfo.position << std::endl;
			*/
			vkCmdBindPipeline(drawCommandBuffers[i], 
				VK_PIPELINE_BIND_POINT_GRAPHICS, obj->graphicsPipeline);
			
			VkDeviceSize vertexOffset[] = { obj->vertexMemoryInfo.position };
			VkBuffer vertexBuffer[] = { objectsBuffers[obj->vertexMemoryInfo.buffer] };
			vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, vertexBuffer, vertexOffset);

			vkCmdBindIndexBuffer(drawCommandBuffers[i], objectsBuffers[obj->indexMemoryInfo.buffer],
				obj->indexMemoryInfo.position, VK_INDEX_TYPE_UINT32);
			
			for (auto push = obj->pushCostants.begin(); push != obj->pushCostants.end(); push++){
				vkCmdPushConstants(drawCommandBuffers[i], obj->graphicsPipelineLayout,
					(*push).second.stageFlags, (*push).second.offset,
					(*push).second.size, (*push).second.values);
			}
			
			vkCmdDrawIndexed(drawCommandBuffers[i], 
				obj->getIndicesData(obj->ownerObject).indexCount, 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(drawCommandBuffers[i]);

		if (vkEndCommandBuffer(drawCommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record draw command buffers");
		}


	}
}