#include "GraphicsEngine.h"



void GraphicsEngine::createObjectsBuffer() {
	VkBuffer objectsBuffer;
	VkDeviceMemory objectsBufferMemory;
	
	try {
		createBuffer(objectsBuffer, objectsBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}
	catch (std::runtime_error err) {
		throw std::runtime_error("failed to create objectsBuffer: " + *err.what());
	}

	objectsBuffers.push_back(objectsBuffer);
	objectsBuffersMemories.push_back(objectsBufferMemory);
	objectsBuffersInfos.resize(objectsBuffersInfos.size() + 1);
}


void GraphicsEngine::createStagingBuffer() {
	try {
		createBuffer(stagingBuffer, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
	catch (std::runtime_error err){
		throw std::runtime_error("failed to create stagingBuffer " + *err.what());
	}
}


void GraphicsEngine::createBuffer(VkBuffer& buffer, VkDeviceMemory& memory,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferAllocationSize;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	
	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory");
	}
	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}


// -------------------------------------------


void GraphicsEngine::destroyObjectsBuffer(size_t index) {
	vkDestroyBuffer(logicalDevice, objectsBuffers[index], nullptr);
	vkFreeMemory(logicalDevice, objectsBuffersMemories[index], nullptr);
	objectsBuffers.erase(objectsBuffers.begin() + index);
	objectsBuffersMemories.erase(objectsBuffersMemories.begin() + index);
	objectsBuffersInfos.erase(objectsBuffersInfos.begin() + index);
}


void GraphicsEngine::destroyStagingBuffer() {
	vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}


// -------------------------------------------


uint32_t GraphicsEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("failed to fund suitable memory");
}