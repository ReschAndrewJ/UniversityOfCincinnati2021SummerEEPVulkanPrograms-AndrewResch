#include "Buffers.h"


// public functions -----------------------------------------

void Buffers::createFrameBuffers(VkDevice logicalDevice, SwapChain& swapChain, GraphicsPipeline& pipeline) {
	swapChainFrameBuffers.resize(swapChain.imageViews.size());

	for (size_t i = 0; i < swapChain.imageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChain.imageViews[i]
		};

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = pipeline.renderPass;
		frameBufferInfo.attachmentCount = 1;
		frameBufferInfo.pAttachments = attachments;
		frameBufferInfo.width = swapChain.extent.width;
		frameBufferInfo.height = swapChain.extent.height;
		frameBufferInfo.layers = 1;
		
		if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer");
		}

	}

}


void Buffers::createCommandPool(Devices& devices) {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = devices.deviceQueueFamilyIndices.graphicsFamilyIndex.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(devices.logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool");
	}
}



void Buffers::createVertexBuffer(Devices& devices, SwapChain& swapChain, const Object& object, int index) {
	if (index != -1) {
		vertexBuffers.insert(vertexBuffers.begin() + index, nullptr);
		vertexBuffersMemory.insert(vertexBuffersMemory.begin() + index, nullptr);
	}
	else {
		index = vertexBuffers.size();
		vertexBuffers.resize(vertexBuffers.size() + 1);
		vertexBuffersMemory.resize(vertexBuffers.size());
	}

	VkDeviceSize bufferSize = sizeof(object.vertices[0]) * object.vertices.size();
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(devices.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, object.vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(devices.logicalDevice, stagingBufferMemory);

	createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffers[index], vertexBuffersMemory[index]);

	copyBuffer(devices, stagingBuffer, vertexBuffers[index], bufferSize);

	vkDestroyBuffer(devices.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(devices.logicalDevice, stagingBufferMemory, nullptr);
}


void Buffers::createIndexBuffer(Devices& devices, const Object& object, int index) {
	if (index != -1) {
		indexBuffers.insert(indexBuffers.begin() + index, nullptr);
		indexBuffersMemory.insert(indexBuffersMemory.begin() + index, nullptr);
	}
	else {
		index = indexBuffers.size();
		indexBuffers.resize(indexBuffers.size() + 1);
		indexBuffersMemory.resize(indexBuffers.size());
	}
	
	VkDeviceSize bufferSize = sizeof(object.indices[0]) * object.indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	
	void* data;
	vkMapMemory(devices.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, object.indices.data(), (size_t)bufferSize);
	vkUnmapMemory(devices.logicalDevice, stagingBufferMemory);

	createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffers[index], indexBuffersMemory[index]);

	copyBuffer(devices, stagingBuffer, indexBuffers[index], bufferSize);

	vkDestroyBuffer(devices.logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(devices.logicalDevice, stagingBufferMemory, nullptr);
}



void Buffers::createCommandBuffers(Devices& devices, SwapChain& swapChain, GraphicsPipeline& pipeline, const std::vector<Object>& objects) {
	commandBuffers.resize(swapChainFrameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(devices.logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}

	recordCommandBuffers(swapChain, pipeline, objects);

}


void Buffers::recordCommandBuffers(SwapChain& swapChain, GraphicsPipeline& pipeline, const std::vector<Object>& objects) {
	for (size_t i = 0; i < commandBuffers.size(); i++) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}
		
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline.renderPass;
		renderPassInfo.framebuffer = swapChainFrameBuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain.extent;

		VkClearValue clearColor = { 0, 0, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		for (size_t index = 0; index < objects.size(); index++) {
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelines[index]);
			VkBuffer vertexBuffers[] = { this->vertexBuffers[index] };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffers[i], indexBuffers[index], 0, VK_INDEX_TYPE_UINT32);
			
			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(objects[index].indices.size()), 1, 0, 0, 0);
		}
		
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}

	}
}


void Buffers::destroyCommandPool(VkDevice logicalDevice) {
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
}


void Buffers::destroyFrameBuffers(VkDevice logicalDevice) {
	for (auto frameBuffer : swapChainFrameBuffers) {
		vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);
	}
}


void Buffers::destroyCommandBuffers(VkDevice logicalDevice) {
	vkFreeCommandBuffers(logicalDevice, commandPool, (uint32_t)commandBuffers.size(), commandBuffers.data());
}


void Buffers::destroyObjectBuffers(VkDevice logicalDevice, size_t index) {
	vkDestroyBuffer(logicalDevice, vertexBuffers[index], nullptr);
	vertexBuffers.erase(vertexBuffers.begin() + index);
	vkFreeMemory(logicalDevice, vertexBuffersMemory[index], nullptr);
	vertexBuffersMemory.erase(vertexBuffersMemory.begin() + index);

	vkDestroyBuffer(logicalDevice, indexBuffers[index], nullptr);
	indexBuffers.erase(indexBuffers.begin() + index);
	vkFreeMemory(logicalDevice, indexBuffersMemory[index], nullptr);
	indexBuffersMemory.erase(indexBuffersMemory.begin() + index);
}



// private functions ----------------------------------------

uint32_t Buffers::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memproperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memproperties);

	for (uint32_t i = 0; i < memproperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && 
		(memproperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to fund suitable memory");

}



void Buffers::createBuffer(Devices devices, VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(devices.logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(devices.logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(devices.physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(devices.logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory");
	}

	vkBindBufferMemory(devices.logicalDevice, buffer, bufferMemory, 0);
}


void Buffers::copyBuffer(Devices devices, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(devices.logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(devices.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(devices.graphicsQueue);

	vkFreeCommandBuffers(devices.logicalDevice, commandPool, 1, &commandBuffer);
}